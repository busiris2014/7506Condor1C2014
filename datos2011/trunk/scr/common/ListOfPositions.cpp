/*
 * ListOfPositions.cpp
 *
 *  Created on: 14/06/2011
 *      Author: luciano
 */

#include "ListOfPositions.h"

ListOfPositions::ListOfPositions(list<int>* listOfPositions,int term) {
	this->listOfPositions = listOfPositions;
	this->term = term;
}

int ListOfPositions::minDistance(list<int>* listToCompare)
{
	list<int>::iterator it;
	list<int>::iterator localIt;
	int min = 1000000000;
	int distance;
	for(it = listToCompare->begin();it != listToCompare->end();++it)
	{
		for(localIt = this->listOfPositions->begin();localIt != this->listOfPositions->end();++localIt)
		{
			distance = (*localIt - *it);
			if((distance < min) && (distance >= 0)) min = distance;
		}
	}
	return min;
}

list<int> *ListOfPositions::getListOfPositions()
{
    return listOfPositions;
}

int ListOfPositions::getTerm()
{
    return term;
}

void ListOfPositions::setTerm(int term)
{
    this->term = term;
}

void ListOfPositions::setListOfPositions(list<int> *listOfPositions)
{
    this->listOfPositions = listOfPositions;
}

ListOfPositions::~ListOfPositions() {
	// TODO Auto-generated destructor stub
}
