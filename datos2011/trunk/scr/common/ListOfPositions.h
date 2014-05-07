#ifndef LISTOFPOSITIONS_H_
#define LISTOFPOSITIONS_H_
#include <list>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

class ListOfPositions {
private:
	list<int>* listOfPositions;
	int term;
public:
	ListOfPositions(list<int>* listOfPositions, int term);
	int minDistance(list<int>* listToCompare);
	virtual ~ListOfPositions();
    list<int> *getListOfPositions();
    void setListOfPositions(list<int> *listOfPositions);
    int getTerm();
    void setTerm(int term);
};

#endif /* LISTOFPOSITIONS_H_ */
