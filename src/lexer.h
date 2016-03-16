#ifndef LEXER_H
#define LEXER_H

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

typedef enum {
	CREATE = 0, VIEW = 1, AS = 2, OUTPUT = 3, SELECT = 4,
	FROM = 5, EXTRACT = 6, REGEX = 7, ON = 8, RETURN = 9,
	GROUP = 10, AND = 11, TOKEN = 12, PATTERN = 13,
	ID = 14, REG = 15, LEFTANGLE = 16, RIGHTANGLE = 17,
	LEFTBRACE = 18, RIGHTBRACE = 19, DOT = 20, COMMA = 21,
	LEFTCIRCLE = 22, RIGHTCIRCLE = 23
} KEYWORDS;

struct Token {
	string content;
	KEYWORDS type;
	Token(string c, KEYWORDS t) {
		content = c;
		type = t;
	}
	Token() {
		content = "";
		type = ID;
	}
};

class Lexer
{
public:
	Lexer();
	~Lexer();
	vector<vector<Token> > generator();
	string getFileName();
	void setFileName(string &name);
private:
	string name; //读取指令文件的文件名
};
#endif