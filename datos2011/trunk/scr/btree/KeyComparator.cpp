#include "KeyComparator.h"

KeyComparator::KeyComparator() {
}

KeyComparator::~KeyComparator() {
}

int KeyComparator::compare(Key k1, Key k2) {
	int compareResult = KeyComparator::alphabeticComparison(k1, k2);
	if (k1.getSize() == k2.getSize()) {
		return compareResult;
	} else {
		Key shortKey;
		if (compareResult == -1) {
			shortKey = k1;
		} else {
			shortKey = k2;
		}

		for (int i = 0; i < shortKey.getSize(); ++i) {
			if (k1.getKey()[i] != k2.getKey()[i]) {
				return compareResult;
			}
		}

		if (shortKey == k1) {
			return 1;
		} else {
			return -1;
		}

	}
}

int KeyComparator::alphabeticComparison(Key k1, Key k2) {
	if (k1 < k2) {
		return -1;
	} else {
		if (k1 > k2) {
			return 1;
		} else {
			return 0;
		}
	}
}
