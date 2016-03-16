#include "lexer.h"
#include <fstream>

//默认读取指令文件的文件名为“Revenue.aql”
Lexer::Lexer() {
	this->name = "Revenue.aql";
}

Lexer::~Lexer() {}

//获取文件名
string Lexer::getFileName() {
	return name;
}

//设置新的文件名
void Lexer::setFileName(string &name) {
	this->name = name;
	return;
}

//判断是否是数字或者字母
bool isNumOrAlp(char c) {
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

//关键字判断，大小写敏感
KEYWORDS isKey(string &input) {
	if (input == "create") return CREATE;
	else if (input == "view") return VIEW;
	else if (input == "as") return AS;
	else if (input == "output") return OUTPUT;
	else if (input == "select") return SELECT;
	else if (input == "from") return FROM;
	else if (input == "extract") return EXTRACT;
	else if (input == "regex") return REGEX;
	else if (input == "on") return ON;
	else if (input == "return") return RETURN;
	else if (input == "group") return GROUP;
	else if (input == "and") return AND;
	else if (input == "Token") return TOKEN;
	else if (input == "pattern") return PATTERN;
	else if (input == "(") return LEFTCIRCLE;
	else if (input == ")") return RIGHTCIRCLE;
	else if (input == "{") return LEFTBRACE;
	else if (input == "}") return RIGHTBRACE;
	else if (input == "<") return LEFTANGLE;
	else if (input == ">") return RIGHTANGLE;
	else if (input == ",") return COMMA;
	else if (input == ".") return DOT;
	else return ID;
}

vector<vector<Token> > Lexer::generator() {
	vector<vector<Token> > output;
	vector<Token> insideToken;
	ifstream inFile;
	char* c = (char*)this->name.data();
	inFile.open(c, ifstream::in);
	string str;
	string input;
	KEYWORDS key;
	while (!inFile.eof()) {
		getline(inFile, input);
		str.clear();
		char chead;
		char crear;
		bool isRegex = false;
		for (int i = 0; i < input.size(); i++) {
			chead = input[i];
			crear = input[i + 1];
			if (isRegex && chead != '/') {
				str.push_back(chead);
			}
			else if (isNumOrAlp(chead)) {
				str.push_back(chead);
				if (!isNumOrAlp(crear)) {
					key = isKey(str);
					Token token(str, key);
					insideToken.push_back(token);
					str.clear();
				}
			}
			else if (chead == ' ') continue;
			else if (chead == ';') {
				output.push_back(insideToken);
				insideToken.clear();
			}
			else if (chead == '\0' || chead == '\n' || chead == '\t') continue;
			else if (chead == '/') {
				if (isRegex) {
					isRegex = false;
					key = REG;
					Token token(str, key);
					insideToken.push_back(token);
					str.clear();
				}
				else isRegex = true;
			}
			else {
				str.push_back(chead);
				key = isKey(str);
				Token token(str, key);
				insideToken.push_back(token);
				str.clear();
			}
		}
	}
	inFile.close();
	return output;
}