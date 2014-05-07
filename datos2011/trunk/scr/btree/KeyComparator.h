
#ifndef KEYCOMPARATOR_H_
#define KEYCOMPARATOR_H_

#include "../common/Key.h"

class KeyComparator {
public:
	KeyComparator();
	virtual ~KeyComparator();

	static int compare(Key k1, Key k2);

private:
	static int alphabeticComparison(Key k1, Key k2);

};

#endif /* KEYCOMPARATOR_H_ */
