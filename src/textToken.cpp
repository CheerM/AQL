#include "textToken.h"

using namespace std;

textToken::textToken(string s, int start, int end) {
	value = string(s);
	posStart = start;
	posEnd = end;
}
//获取起始位置
int textToken::getPosStart() {
	return posStart;
}
//获取结尾位置
int textToken::getPosEnd() {
	return posEnd;
}
//获取token的字符串值
string textToken::getValue() {
	return value;
}
//将token输出 e.g. carter(0, 6)
string textToken::toString() {
	string s = value + ":(" + to_string(posStart) + "," + to_string(posEnd) + ")";
	return s;
}

