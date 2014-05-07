#include "Term.h"

Term::Term() {
}

Term::Term(string word, unsigned int firstPosition) {
	this->word = word;
	this->frecuency = 1;
	this->positions.push_back(firstPosition);
}

Term::~Term() {
}

void Term::setWord(string word)
{
}

string Term::getWord()
{
	return this->word;
}

list<unsigned int> Term::getPositions()
{
	return this->positions;
}

void Term::addPosition(unsigned int position)
{
	this->positions.push_back(position);
	this->increaseFrecuency();
}

void Term::increaseFrecuency()
{
	this->frecuency = this->frecuency + 1;
}

unsigned int Term::getFrecuency()
{
	return this->frecuency;
}
