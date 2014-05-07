#ifndef WORD_H_
#define WORD_H_

#include <string>
#include <list>
#include "../common/List.h"

using namespace std;

class Word {
private:
	string word;
	int idWord;
	list<int>* books;


public:
	Word();
	Word(string word, list<int>* book);
	virtual ~Word();

	string getWord();
	int getIdWord();
	list<int>* getBooks();
};

#endif /* WORD_H_ */
