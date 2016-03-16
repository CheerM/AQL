#include "Tokenizer.h"

Tokenizer::Tokenizer()
{
	tokenTable = new vector<textToken>();
}

Tokenizer::~Tokenizer()
{
	if (tokenTable != NULL) {
		vector<textToken> *temp = tokenTable;
		delete temp;
		tokenTable = NULL;
	}
}
//获取文本内容
string Tokenizer::getTextContent() {
	return textContent;
}
//获取token表
vector<textToken> Tokenizer::getTokenTable() {
	return *tokenTable;
}
//读取文本  
bool Tokenizer::readFromFile(const char *fpath) {
	ifstream in;
	in.open(fpath);
	if (!in.is_open()) {
		return false;
	}
	int start = 0;
	int end = 0;
	while (in.peek() != EOF) {
		char nextChar = in.peek();
		if (isWhiteSpace(nextChar) || isDrop(nextChar)) {
			textContent.push_back(nextChar);
			in.get();
			end++;
			start = end;
			continue;
		}
		if (isSpecial(nextChar)) {
			textContent.push_back(nextChar);
			char c = in.get();
			string temp;
			temp.push_back(c);
			end++;
			textToken token(temp, start, end);
			tokenTable->push_back(token);
			start = end;
			continue;
		}
		string temp;
		while (isDigit(nextChar) || isLetter(nextChar)) {
			textContent.push_back(nextChar);
			char c = in.get();
			temp.push_back(c);
			end++;
			nextChar = in.peek();
		}
		textToken token(temp, start, end);
		tokenTable->push_back(token);
		start = end;
	}
	in.close();
	return true;
}
//将token写入文件
bool Tokenizer::writeToFile(const char *fpath) {
	ofstream out;
	out.open(fpath);
	if (out.is_open()) {
		for (vector<textToken>::iterator it = tokenTable->begin(); it != tokenTable->end(); it++) {
			out << it->toString() << endl;
		}
		out.close();
		return true;
	}
	return false;
}
//是否数字
bool Tokenizer::isDigit(char c) {
	return c >= '0' && c <= '9';
}
//是否字母
bool Tokenizer::isLetter(char c) {
	return ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'));
}
//是否空格
bool Tokenizer::isWhiteSpace(char c) {
	return c == ' ';
}
//是否特殊符号
bool Tokenizer::isSpecial(char c) {
	return (!isDigit(c) && !isLetter(c) && !isWhiteSpace(c));
}

bool Tokenizer::isDrop(char c) {
	return (c == '\n' || c == '\t');
}