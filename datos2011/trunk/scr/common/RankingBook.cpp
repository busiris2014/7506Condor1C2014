/*
 * RankingBook.cpp
 *
 *  Created on: 14/06/2011
 *      Author: luciano
 */

#include "RankingBook.h"

RankingBook::RankingBook() {
	// TODO Auto-generated constructor stub

}

int RankingBook::getDistanceTerms() const
{
    return distanceTerms;
}

int RankingBook::getIdBook() const
{
    return idBook;
}

void RankingBook::setDistanceTerms(int distanceTerms)
{
    this->distanceTerms = distanceTerms;
}

void RankingBook::setIdBook(int idBook)
{
    this->idBook = idBook;
}

RankingBook::~RankingBook() {
	// TODO Auto-generated destructor stub
}
