#include "parse.h"
#include <set>
#include <iomanip>

extern vector<vector<int> > findall(const char *regex, const char *content);

parse::parse() {
	table_name = "";
}
parse::parse(string name, vector<column> c) {
	setTableName(name);
	setTableCols(c);
}
parse::parse(vector<textToken> text, vector<Token> token, string textcontent) {
	txttoken = text;
	aqltoken = token;
	textContent = textcontent;
}
parse::~parse() {
	groupnames.clear();   //解析所得组的组号和组名
	groups.clear();      //解析所得组的内容
	matchs.clear();      //记录匹配用到的内容
	aqltoken.clear();     //aqltoken输入
	tables.clear();       //可供查找的已构造好的表
}
void parse::createTable() {
	int start = 0;
	int end = aqltoken.size() - 1;
	if (aqltoken.size() >= 5) {
		if (aqltoken[start].type == KEYWORDS::CREATE && aqltoken[start + 1].type == KEYWORDS::VIEW) {
			if (aqltoken[start + 2].type == KEYWORDS::ID) {
				setTableName(aqltoken[start + 2].content);
				if (aqltoken[start + 3].type == KEYWORDS::AS) {
					start += 4;
					viewStmt(start, end);
				}
				else {
					cout << "your code is wrong at the CREATE VIEW ... AS" << endl;
				}
			}
			else {
				cout << "your code is wrong at the CREATE VIEW" << endl;
			}
		}

	}
	else {
		cout << "your code is wrong" << endl;
	}
}
void parse::viewStmt(int start, int end) {
	int temp = start;
	while (aqltoken[temp].type != KEYWORDS::FROM) temp++;
	if (temp != end) {
		if (aqltoken[start].type == KEYWORDS::EXTRACT) extractStmt(start + 1, temp - 1);
		else if (aqltoken[start].type == KEYWORDS::SELECT) selectList(start + 1, temp - 1);
	}
	else {
		cout << "There is no table to deal with" << endl;
	}
}
void parse::extractStmt(int start, int end) {
	int half = start;
	for (; half <= end; half++) {
		if (aqltoken[half].type == KEYWORDS::AS || aqltoken[half].type == KEYWORDS::RETURN) break;
	}
	if (aqltoken[start].type == KEYWORDS::REGEX) {
		vector<column> get_col = regexSpec(start + 1, half);
		if (get_col.size() != 0) {
			nameSpec(get_col, half, end);
		}
	}
	else if (aqltoken[start].type == KEYWORDS::PATTERN) {
		pattern_spec(start, end);
	}
}
vector<column> parse::regexSpec(int start, int end) {
	vector<column> out;
	if (aqltoken[start].type == 15) {
		char* regex = (char*)aqltoken[start].content.data();
		start++;
		if (aqltoken[start].type == KEYWORDS::ON && aqltoken[start + 1].type == KEYWORDS::ID && aqltoken[start + 2].type == KEYWORDS::DOT && aqltoken[start + 3].type == KEYWORDS::ID) {
			bool find = false;
			column from_col;
			//找到表格的名字
			string from_table_name = aqltoken[start + 1].content;
			//找到表格中的列
			for (int i = 0; i < tables.size(); i++) {
				if (tables[i].getTableName() == from_table_name) {
					vector<column> temp = tables[i].getTableCol();
					for (int j = 0; j < temp.size(); j++) {
						if (temp[j].columnname == aqltoken[start + 3].content) {
							from_col = temp[j];
							find = true;
							break;
						}
					}
				}
			}
			if (find == true) {
				vector<column> output;
				vector<pair<int, vector<textToken> > > output_row;
				for (int i = 0; i < from_col.content.size(); i++) {
					string content = from_col.content[i].getValue();
					int start1 = from_col.content[i].getPosStart();
					vector<vector<int> > temp = findall(regex, (char*)content.data());
					for (int ii = 0; ii < temp.size(); ii++) {
						if (output_row.size() != temp[0].size() / 2) {
							for (int l = 0; l < temp[0].size() / 2; l++) {
								vector<textToken> temp1;
								output_row.push_back(pair<int, vector<textToken> >(0, temp1));
							}
						}
						for (int j = 0; j < temp[ii].size(); j += 2) {
							string str = "";
							for (int l = temp[ii][j]; l < temp[ii][j + 1]; l++) str += content[l];
							output_row[j / 2].second.push_back(textToken(str, start1 + temp[ii][j], start1 + temp[ii][j + 1]));
							if (str.size() > output_row[j / 2].first) output_row[j / 2].first = str.size();
						}
					}
				}
				if (output_row.size() > 0) {
					for (int i = 0; i < output_row.size(); i++) {
						output.push_back(column(from_col.columnname, output_row[i].second));
					}
					return output;
				}
				else {
					cout << "there is no match" << endl;
				}
			}
			else {
				cout << "your code is wrong at the ON ID.ID" << endl;
			}
		}
		else {
			cout << "your code is wrong at the EXTRACT REGEX REG ON ..." << endl;
		}
	}
	return out;
}
void parse::selectList(int start, int end) {
	if (start + 2 <= end) {
		if (aqltoken[start].type == 14 && aqltoken[start + 1].type == 20 && aqltoken[start + 2].type == 14) {
			string str_name = aqltoken[start].content;
			if (tables[0].getTableName() == str_name) {
				vector<column> get_table = tables[0].getTableCol();
				bool find = false;
				column found;
				for (int i = 0; i < get_table.size(); i++) {
					if (aqltoken[start + 2].content == get_table[i].columnname) {
						find = true;
						found = get_table[i];
						break;
					}
				}
				start += 3;
				if (start + 1 <= end) {
					if (aqltoken[start].type == 2 && aqltoken[start + 1].type == 14) {
						found.columnname = aqltoken[start + 1].content;
						start += 2;
					}
				}
				if (find == true) setTableCol(found);
				if (start + 1 < end) {
					selectList(start + 1, end);
				}
			}
		}
	}
}
//根据表名获取表
parse parse::getTable(string tablename) {
	for (int i = 0; i < tables.size(); i++) {
		if (tables.at(i).getTableName() == tablename) {
			return tables.at(i);
		}
	}
}
//返回pattern语句解析结果
void parse::pattern_spec(int start, int end) {
	int index = start;
	while (index < end) {
		Token temp = aqltoken.at(index);
		if (temp.type == KEYWORDS::RETURN) {
			break;
		}
		index++;
	}
	column group0 = pattern_expr(start + 1, index - 1);
	groupnames = name_spec(index, end);

	//修改开始
	//在此处修改列名
	for (int i = 0; i < groups.size(); i++) {
		for (int j = 0; j < matchs.size(); j++) {
			if (matchs[j].columnname == groups[i].columnname) {
				groups[i] = column(groups[i].columnname, matchs[j].content);
				break;
			}
		}
	}
	groups.insert(groups.begin(), group0);
	vector<column> res;
	for (int i = 0; i < groupnames.size(); i++) {
		group g = groupnames.at(i);
		int pos = g.groupid;
		column col = groups.at(pos);
		col.columnname = g.groupname;
		res.push_back(col);
	}
	//修改结束
	col = res;
}
//返回pattern_expr解析后的列内容
column parse::pattern_expr(int start, int end) {
	int index = start;
	Token temp = aqltoken.at(index);
	int pattern_pkg_satrt = index;
	if (temp.type == KEYWORDS::LEFTCIRCLE) {
		pattern_pkg_satrt = index;
		while (temp.type != KEYWORDS::RIGHTCIRCLE) {
			index++;
			temp = aqltoken.at(index);
		}
	}
	else if (temp.type == KEYWORDS::LEFTANGLE) {
		pattern_pkg_satrt = index;
		while (temp.type != KEYWORDS::RIGHTANGLE) {
			index++;
			temp = aqltoken.at(index);
		}
	}
	else if (temp.type == KEYWORDS::REG) {
		pattern_pkg_satrt = index;
	}
	int pattern_pkg_end = index;
	if (pattern_pkg_end < end) {
		int num_index = index + 1;
		temp = aqltoken.at(num_index);
		if (temp.type == KEYWORDS::LEFTBRACE) {
			while (temp.type != KEYWORDS::RIGHTBRACE)
			{
				num_index++;
				temp = aqltoken.at(num_index);
			}
			pattern_pkg_end = num_index;
			index = num_index;
		}
	}
	column col1 = pattern_pkg(pattern_pkg_satrt, pattern_pkg_end);
	if (pattern_pkg_end == end) {
		return col1;
	}
	column col2 = pattern_expr(index + 1, end);

	//修改开始
	vector<textToken> content = match(col1.content, col2.content, col1.columnname, col2.columnname);
	//修改结束

	column col3("temp", content);
	return col3;
}
//匹配后改变组的内容 使被捕获的子组的内容和匹配用到的子组内容相同
//返回pattern_pkg解析后的列内容
column parse::pattern_pkg(int start, int end) {
	int index = start;
	Token temp = aqltoken.at(index);
	if (temp.type == KEYWORDS::LEFTCIRCLE) {
		int pattern_group_satrt = index;
		while (temp.type != KEYWORDS::RIGHTCIRCLE) {
			index++;
			temp = aqltoken.at(index);
		}
		return pattern_group(pattern_group_satrt, index);
	}
	int atom_expr_satrt = index;
	if (temp.type == KEYWORDS::LEFTANGLE) {
		atom_expr_satrt = index;
		while (temp.type != KEYWORDS::RIGHTANGLE) {
			index++;
			temp = aqltoken.at(index);
		}
	}
	else if (temp.type == KEYWORDS::REG) {
		atom_expr_satrt = index;
	}
	column col = atom_expr(atom_expr_satrt, index);
	if (index == end) {
		return col;
	}
	index = index + 2;
	temp = aqltoken.at(index);
	int min = atoi(temp.content.c_str());
	index = index + 2;
	temp = aqltoken.at(index);
	int max = atoi(temp.content.c_str());
	vector<textToken> content;
	//修改开始
	if (col.columnname == "token") {
		int size = txttoken.size();
		for (int i = min; i <= max; i++) {
			for (int k = 0; k < size - i; k++) {
				int start = txttoken[k].getPosStart();
				int end = txttoken[k + i - 1].getPosEnd();
				string val = textContent.substr(start, end - start);
				content.push_back(textToken(val, start, end));
			}
			;
		}
	}
	else {
		//重复min, max
		vector<textToken> left = col.content;
		vector<textToken> right = col.content;
		for (int cnt = 0; cnt < min - 1; cnt++) {
			left = match(left, right);
		}
		for (int cnt = min; cnt <= max; cnt++) {
			content.insert(content.end(), left.begin(), left.end());
			left = match(left, right);
		}
		//重复min, max
	}

	column res(col.columnname, content);
	//修改结束
	return res;
}
//对2个列进行匹配
vector<textToken> parse::match(vector<textToken> content1, vector<textToken> content2) {
	vector<textToken> content;
	for (int i = 0; i < content1.size(); i++) {
		textToken temp1 = content1.at(i);
		int posStart = temp1.getPosStart();
		int posLast = temp1.getPosEnd();
		for (int j = 0; j < content2.size(); j++) {
			textToken temp2 = content2.at(j);
			if (posLast == temp2.getPosStart()) {
				textToken temp3(temp1.getValue() + temp2.getValue(), posStart, temp2.getPosEnd());
				content.push_back(temp3);
			}
			if (posLast + 1 == temp2.getPosStart()) {
				textToken temp3(temp1.getValue() + " " + temp2.getValue(), posStart, temp2.getPosEnd());
				content.push_back(temp3);
			}
		}
	}
	return content;
}
//对2个列进行匹配
vector<textToken> parse::match(vector<textToken> content1, vector<textToken> content2, string name1, string name2) {
	vector<textToken> newcontent1;
	vector<textToken> newcontent2;
	vector<textToken> content;
	set<int> matchcontent2;
	for (int i = 0; i < content1.size(); i++) {
		textToken temp1 = content1.at(i);
		int posStart = temp1.getPosStart();
		int posLast = temp1.getPosEnd();
		bool find = false;
		for (int j = 0; j < content2.size(); j++) {
			textToken temp2 = content2.at(j);
			int ft = matchcontent2.count(j);
			if (posLast == temp2.getPosStart()) {
				textToken temp3(temp1.getValue() + temp2.getValue(), posStart, temp2.getPosEnd());
				find = true;
				if (ft == 0) {
					matchcontent2.insert(j);
					newcontent2.push_back(temp2);
				}
				content.push_back(temp3);
			}
			if (posLast + 1 == temp2.getPosStart()) {
				textToken temp3(temp1.getValue() + " " + temp2.getValue(), posStart, temp2.getPosEnd());
				find = true;
				if (ft == 0) {
					matchcontent2.insert(j);
					newcontent2.push_back(temp2);
				}
				content.push_back(temp3);
			}
		}
		if (find) {
			newcontent1.push_back(temp1);
		}
	}

	//修改开始
	if (matchs.size() == 0) {
		matchs.push_back(column(name1, newcontent1));
		matchs.push_back(column(name2, newcontent2));
	}
	else {
		for (int i = 0; i < matchs.size(); i++) {
			column col = matchs[i];
			vector<textToken> oldc = col.content;
			vector<textToken> newc;
			for (set<int>::iterator it = matchcontent2.begin(); it != matchcontent2.end(); it++) {
				newc.push_back(oldc[*it]);
			}
			matchs[i] = column(col.columnname, newc);
		}
		matchs.push_back(column(name1, newcontent1));
	}
	//修改结束
	return content;
}
//返回pattern_group解析后的列内容 并将解析所得列存入记录组的容器中
column parse::pattern_group(int start, int end) {
	column col = pattern_expr(start + 1, end - 1);
	groups.push_back(col);
	return col;
}
//返回atom_expr解析后的列内容 可为表格的列 Token 正则
column parse::atom_expr(int start, int end) {
	int index = start;
	Token temp = aqltoken.at(index);
	if (temp.type == KEYWORDS::LEFTANGLE) {
		index++;
		temp = aqltoken.at(index);
		if (temp.type == KEYWORDS::TOKEN) {
			return column("token", txttoken);
		}
		else if (temp.type == KEYWORDS::ID) {
			string tablename = temp.content;
			index = index + 2;
			temp = aqltoken.at(index);
			string columnname = temp.content;
			parse tabletmp = getTable(tablename);
			return getColumn(tabletmp, columnname);
		}
	}
	else if (temp.type == KEYWORDS::REG) {
		vector< vector<int> > find = findall(temp.content.c_str(), textContent.c_str());
		vector<textToken> content;
		for (int i = 0; i < find.size(); i++) {
			vector<int> temp = find.at(i);
			string val = textContent.substr(temp[0], temp[1] - temp[0]);
			content.push_back(textToken(val, temp[0], temp[1]));
		}
		return column("reg", content);
	}
}
//根据列名获取列内容
column  parse::getColumn(parse temp, string colname) {
	vector<column> col = temp.col;
	for (int i = 0; i < col.size(); i++) {
		if (col.at(i).columnname == colname) {
			return col.at(i);
		}
	}
}
//返回解析所得组信息 即组对应的组号和组名
vector<group> parse::name_spec(int start, int end) {
	int index = start;
	return group_spec(index + 1, end);
}
//返回group_spec解析后的所有组内容
vector<group> parse::group_spec(int start, int end) {
	int index = start;
	while (index < end) {
		Token temp = aqltoken.at(index);
		if (temp.type == KEYWORDS::GROUP) {
			int single_group_satrt = index;
			while (temp.type != KEYWORDS::AND) {
				index++;
				if (index > end) {
					break;
				}
				temp = aqltoken.at(index);
			}
			group groupname = single_group(single_group_satrt, index - 1);
			groupnames.push_back(groupname);
		}
		index++;
	}
	return groupnames;
}
////返回single_group解析后的组内容 即单个组的内容 包括组的名称及其对应的列内容
group parse::single_group(int start, int end) {
	int groupid = atoi(aqltoken.at(start + 1).content.c_str());
	string groupname = aqltoken.at(end).content;
	return group(groupid, groupname);
}
void parse::setTableName(string name) {
	table_name = name;
}
void parse::setTableCol(column c) {
	col.push_back(c);
}
void parse::setTableCols(vector<column> c) {
	col = c;
}
void parse::setNeedTables(vector<parse> from) {
	tables = from;
}
string parse::getTableName() {
	return table_name;
}
vector<column> parse::getTableCol() {
	return col;
}
void parse::nameSpec(vector<column> input, int start, int end) {
	if (aqltoken[start].type == 2 && aqltoken[start + 1].type == 14){
		column temp = input[0];
		temp.columnname = aqltoken[start + 1].content;
		setTableCol(temp);
	}
	else if (aqltoken[start].type == 9 || aqltoken[start].type == 11) {
		if (aqltoken[start + 1].type == 10 && aqltoken[start + 2].type == 14) {
			vector<column> output;
			int num = std::atoi(aqltoken[start + 2].content.c_str());
			if (num <= input.size()) {
				output.push_back(input[num]);
				nameSpec(output, start + 3, start + 4);
			}
			start += 5;
			if (aqltoken[start].content == "and") {
				nameSpec(input, start, end);
			}
		}
	}
	else {
		return;
	}
}
void parse::printTable(string name) {
	if (name == "") name = this->getTableName();
	cout << "View: " << name << endl;
	vector<column> col_infor = this->getTableCol();
	if (col_infor.size() != 0) {
		if (col_infor[0].content.size() != 0) {
			vector<int> max(col_infor.size(), 1);
			for (int i = 0; i < col_infor.size(); i++) {
				if (col_infor[i].columnname.size() > max[i]) max[i] = col_infor[i].columnname.size();
			}
			for (int i = 0; i < col_infor[0].content.size(); i++) {
				for (int j = 0; j < col_infor.size(); j++) {
					if (col_infor[j].content[i].toString().size() > max[j]) {
						max[j] = col_infor[j].content[i].toString().size();
					}
				}
			}
			//一行一行输出，考虑到多个列的情况
			//列名上边框
			for (int i = 0; i < col_infor.size(); i++) {
				cout << std::setfill('-') << std::setw(max[i] + 1) << std::left << "+";
			}
			cout << "+" << endl;
			//列名
			for (int i = 0; i < col_infor.size(); i++) {
				cout << "|" << std::setfill(' ') << std::setw(max[i]) << std::left << col_infor[i].columnname;
			}
			cout << "|" << endl;
			//列名下边框
			for (int i = 0; i < col_infor.size(); i++) {
				cout << std::setfill('-') << std::setw(max[i] + 1) << std::left << "+";
			}
			cout << "+" << endl;
			//行信息
			for (int i = 0; i < col_infor[0].content.size(); i++) {
				for (int j = 0; j < col_infor.size(); j++) {
					cout << "|" << std::setw(max[j]) << std::setfill(' ') << std::left << col_infor[j].content[i].toString();
				}
				cout << std::right << "|" << endl;
			}
			//表格下边框
			for (int i = 0; i < col_infor.size(); i++) {
				cout << std::setfill('-') << std::setw(max[i] + 1) << std::left << "+";
			}
			cout << "+" << endl;
			//表格行数信息显示
		}
		if (col_infor[0].content.size() > 1) {
			cout << col_infor[0].content.size() << " rows in set" << endl;
		}
		else if (col_infor[0].content.size() == 1){
			cout << col_infor[0].content.size() << " row in set" << endl;
		}
		else {
			cout << "Empty set" << endl;
		}
	}
	else {
		cout << "Empty set" << endl;
	}
	cout << endl;
}

