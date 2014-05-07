
#ifndef BPLUSTREEITERATOR_H_
#define BPLUSTREEITERATOR_H_

#include "LeafNode.h"
#include "../common/Record.h"
#include "../common/FileBlockManager.h"

class BPlusTreeIterator {

private:
	LeafNode* nodoActual;
	int posicionActual;
	FileBlockManager* persistor;

public:
	BPlusTreeIterator();
	BPlusTreeIterator(LeafNode *hojaCorriente, int posicionActual, FileBlockManager* persistor);
	virtual ~BPlusTreeIterator();
	bool hasNext();
	Record* next();
};

#endif /* BPLUSTREEITERATOR_H_ */
