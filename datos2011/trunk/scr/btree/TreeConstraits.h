#ifndef TREECONSTRAITS_H_
#define TREECONSTRAITS_H_

#include "../common/Resource.h"

class TreeConstraits {

public:
	TreeConstraits();
	virtual ~TreeConstraits();
	static int getFullSizeNode();
	static int getControlSizeNode();
	static int getEfectiveSizeNode();
	static int getControlSizeRecord();
	static int getPercentRecord();
};

#endif /* TREECONSTRAITS_H_ */
