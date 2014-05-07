#ifndef BOOKGLOBALWEIGHT_H_
#define BOOKGLOBALWEIGHT_H_

class BookGlobalWeight {
private:
	int idBook;
	double globalWeight;
public:
	BookGlobalWeight();
	BookGlobalWeight(int idBook, double globalWeight);
	virtual ~BookGlobalWeight();

	int getIdBook();
	double getGlobalWeight();
};

#endif /* BOOKGLOBALWEIGHT_H_ */
