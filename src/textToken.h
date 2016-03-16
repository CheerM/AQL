#ifndef TEXTTOKEN_H
#define TEXTTOKEN_H

#include <string>
using namespace std;

class textToken {
	private:
		string value; //token���ַ���ֵ
		int posStart; //��ʼλ��
		int posEnd;   //��βλ��
	public:
		textToken(string s, int start, int end);
		int getPosStart();
		int getPosEnd();
		string getValue();
		string toString();
};
#endif