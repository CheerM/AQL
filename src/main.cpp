#include "parse.h"
#include <dirent.h>

void runningProgram(string inputname, string temp, string aqlname) {

	vector<parse> table_set;

	Tokenizer tokenizer;
	bool ok = tokenizer.readFromFile((inputname + temp).data());
	vector<textToken> inputTextToken = tokenizer.getTokenTable();
	string inputText = tokenizer.getTextContent();
	vector<column> document_col;
	document_col.push_back(column("text", inputTextToken));
	parse document("Document", document_col);
	table_set.push_back(document);
	Lexer lexer;
	lexer.setFileName(aqlname);
	vector<vector<Token> > parse_list = lexer.generator();
	cout << "Processing "<< temp << endl;
	for (int i = 0; (size_t)i < parse_list.size(); i++) {
		if (parse_list[i][0].type == KEYWORDS::CREATE) {
			int length = parse_list[i].size() - 1;
			while (parse_list[i][length].type != KEYWORDS::FROM && length >= 0) {
				length--;
			}
			if (length < 0) {
				cout << "There is no source to deal width" << endl;
			}
			else {
				//fromlist
				vector<parse> need_list;
				for (int j = length + 1; (size_t)j < parse_list[i].size();) {
					if (parse_list[i][j].type == KEYWORDS::ID && parse_list[i][j + 1].type == KEYWORDS::ID) {
						string get_table_name = parse_list[i][j].content;
						parse need;
						for (int l = 0; (size_t)l < table_set.size(); l++) {
							if (table_set[l].getTableName() == get_table_name) {
								need = table_set[l];
								need.setTableName(parse_list[i][j + 1].content);
								break;
							}
						}
						if (need.getTableName() != "") {
							need_list.push_back(need);
						}
						j += 3;
					}
				}
				if (need_list.size() != 0) {
					parse newOne(inputTextToken, parse_list[i], inputText);
					newOne.setNeedTables(need_list);
					newOne.createTable();
					table_set.push_back(newOne);
				}
				else {
					cout << "there is no table to deal with" << endl;
				}
			}
		}
		else if (parse_list[i][0].type == KEYWORDS::OUTPUT) {
			if (parse_list[i][1].type == KEYWORDS::VIEW) {
				string get_table_name = parse_list[i][2].content;
				parse need;
				for (int j = 0; (size_t)j < table_set.size(); j++) {
					if (table_set[j].getTableName() == get_table_name) {
						need = table_set[j];
						break;
					}
				}
				if (need.getTableName() != "") {
					string name = "";
					if (parse_list[i].size() == 5) {
						if (parse_list[i][3].type == KEYWORDS::AS && parse_list[i][4].type == KEYWORDS::ID) {
							name = parse_list[i][4].content;
							need.printTable(name);
						}
					}
					else {
						need.printTable(name);
					}
				}
			}
		}
		else {
			cout << endl;
		}
	}
}

int main(int argc, char* arg[]) {
	if (argc == 3) {
		string aqlname(arg[1]);
		DIR* files = NULL;
		struct dirent* ent = NULL;
		files = opendir(arg[2]);
		string inputname = string(arg[2]) + "/";
		if (files == NULL) {
			cout << "There is no file in the direct." << endl;
		} else {
			while ((ent = readdir(files)) != NULL) {
				string temp(ent->d_name);
				if (temp.find(".txt") != -1 || temp.find(".input") != -1) {
					runningProgram(inputname, temp, aqlname);
				}
			}
		}
	}
	return 0;
}
