#include "Tokenizer.h"
#include "lexer.h"

struct column
{
	string columnname;
	vector<textToken> content;
	column() {
		columnname = "";
	}
	column(string s, vector<textToken> c) {
		columnname = s;
		content = c;
	}
};

//输出的组 含组号和组名
struct group {
	int groupid;
	string groupname;
	group(int gid, string n) {
		groupid = gid;
		groupname = n;
	}
};

class parse {
private:
	vector<group> groupnames;   //解析所得组的组号和组名
	vector<column> groups;      //解析所得组的内容
	vector<column> matchs;      //记录匹配用到的内容
	vector<Token> aqltoken;     //aqltoken输入
	vector<textToken> txttoken; //文本token
	vector<parse> tables;       //可供查找的已构造好的表
	string textContent;         //文本内容
	string table_name;
	vector<column> col;

public:

	//创建新的表格
	parse();
	parse(string name, vector<column> c);
	parse(vector<textToken> text, vector<Token> token, string textcontent);
	~parse();
	//create
	void createTable();
	void viewStmt(int start, int end);
	//extract REG
	void extractStmt(int start, int end);
	void nameSpec(vector<column> input, int start, int end);
	vector<column> regexSpec(int start, int end);
	//select
	void selectList(int start, int end);
	//pattern
	vector<textToken> match(vector<textToken> content1, vector<textToken> content2, string name1, string name2);
	vector<textToken> match(vector<textToken> content1, vector<textToken> content2);
	void pattern_spec(int start, int end);
	column pattern_expr(int start, int end);
	column pattern_pkg(int start, int end);
	column pattern_group(int start, int end);
	column atom_expr(int start, int end);
	vector<group> name_spec(int start, int end);
	vector<group> group_spec(int start, int end);
	group single_group(int start, int end);
	//setting
	void setTableName(string name);
	void setTableCol(column c);
	void setTableCols(vector<column> Col);
	void setNeedTables(vector<parse> from);
	string getTableName();
	vector<column> getTableCol();
	//print
	void printTable(string name);

	//缺少构造好的表 用以下函数构造一个表
	//void inittables();
	void fromlist(vector<parse> table);
	parse getTable(string tablename);
	column getColumn(parse temp, string colname);
};