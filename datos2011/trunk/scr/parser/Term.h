#ifndef TERM_H_
#define TERM_H_

#include <string>
#include <list>

using namespace std;

class Term {

private:
	string word;
	list<unsigned int> positions;
	unsigned int frecuency;

	void increaseFrecuency();

public:

	Term();
	Term(string word, unsigned int firstPosition);
	virtual ~Term();

	void setWord(string word);
	string getWord();

	list<unsigned int> getPositions();

	void addPosition(unsigned int position);

	unsigned int getFrecuency();

};

#endif /* TERM_H_ */
