/*
 * RankingBook.h
 *
 *  Created on: 14/06/2011
 *      Author: luciano
 */

#ifndef RANKINGBOOK_H_
#define RANKINGBOOK_H_

class RankingBook {
private:
	int idBook;
	int distanceTerms;
public:
	RankingBook();
	virtual ~RankingBook();
    int getDistanceTerms() const;
    int getIdBook() const;
    void setDistanceTerms(int distanceTerms);
    void setIdBook(int idBook);
};

#endif /* RANKINGBOOK_H_ */
