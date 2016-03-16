#ifndef TEXTTOKEN_H
#define TEXTTOKEN_H

#include <string>
using namespace std;

class textToken {
	private:
		string value; //token的字符串值
		int posStart; //起始位置
		int posEnd;   //结尾位置
	public:
		textToken(string s, int start, int end);
		int getPosStart();
		int getPosEnd();
		string getValue();
		string toString();
};
#endif
