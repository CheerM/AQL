AQL: main.o parse.o regex.o Tokenizer.o textToken.o lexer.o
	g++ -std=c++11 main.o parse.o regex.o Tokenizer.o textToken.o lexer.o -o AQL

main.o: main.cpp regex.cpp parse.h Tokenizer.h lexer.h textToken.h
	g++ -c -std=c++11 main.cpp -o main.o -g

parse.o: parse.cpp regex.cpp parse.h Tokenizer.h lexer.h textToken.h
	g++ -c -std=c++11 parse.cpp -o parse.o -g

Tokenizer.o: Tokenizer.cpp Tokenizer.h textToken.h
	g++ -c -std=c++11 Tokenizer.cpp -o Tokenizer.o -g

textToken.o: textToken.cpp textToken.h
	g++ -c -std=c++11 textToken.cpp -o textToken.o -g

lexer.o: lexer.cpp lexer.h
	g++ -c -std=c++11 lexer.cpp -o lexer.o -g

regex.o: regex.cpp
	g++ -c -std=c++11 regex.cpp -o regex.o -g

clean:
	rm main *.o -f


