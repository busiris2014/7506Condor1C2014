#include "Word.h"

Word::Word() {
	this->word = "";
	this->books = new list<int>();
}

Word::Word(string word, list<int>* book) {
	this->word = word;

	if (book->size() >= 2)
	{
		this->idWord = book->back() * (-1);
		//Elimina el id de termino contenido en la list
		book->remove(this->idWord * (-1));
		this->books = book;
	}
	else
	{
		this->idWord = 0;
		this->books = new list<int>();
	}
}

Word::~Word() {
	delete (this->books);
}

string Word::getWord()
{
	return this->word;
}

int Word::getIdWord()
{
	return this->idWord;
}

list<int>* Word::getBooks()
{
	return this->books;
}
