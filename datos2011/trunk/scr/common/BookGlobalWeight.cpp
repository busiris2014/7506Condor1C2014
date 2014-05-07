#include "BookGlobalWeight.h"

BookGlobalWeight::BookGlobalWeight() {
}

BookGlobalWeight::BookGlobalWeight(int idBook, double globalWeight) {
	this->idBook = idBook;
	this->globalWeight = globalWeight;
}

BookGlobalWeight::~BookGlobalWeight() {
	// TODO Auto-generated destructor stub
}

int BookGlobalWeight::getIdBook()
{
	return this->idBook;
}

double BookGlobalWeight::getGlobalWeight()
{
	return this->globalWeight;
}

