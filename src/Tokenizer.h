#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <fstream>
#include <vector>
#include "textToken.h"

class Tokenizer
{
public:

	Tokenizer();
	~Tokenizer();

	bool readFromFile(const char *fpath);
	bool writeToFile(const char *fpath);
	bool isDigit(char c);
	bool isLetter(char c);
	bool isWhiteSpace(char c);
	bool isSpecial(char c);
	bool isDrop(char c);
	vector<textToken> getTokenTable();
	string getTextContent();


private:
	vector<textToken> *tokenTable; //��������token��
	string textContent; //�ı�����
};

#endif