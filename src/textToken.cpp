#include "textToken.h"

using namespace std;

textToken::textToken(string s, int start, int end) {
	value = string(s);
	posStart = start;
	posEnd = end;
}
//��ȡ��ʼλ��
int textToken::getPosStart() {
	return posStart;
}
//��ȡ��βλ��
int textToken::getPosEnd() {
	return posEnd;
}
//��ȡtoken���ַ���ֵ
string textToken::getValue() {
	return value;
}
//��token��� e.g. carter(0, 6)
string textToken::toString() {
	string s = value + ":(" + to_string(posStart) + "," + to_string(posEnd) + ")";
	return s;
}
