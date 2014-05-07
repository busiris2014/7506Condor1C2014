#ifndef NODE_H_
#define NODE_H_

#include "../common/Key.h"
#include "../common/Constantes.h"
#include "../common/ByteString.h"
#include "../common/Serializer.h"
#include "TreeConstraits.h"


class Node : public Serializer{
	friend class BPlusTree;
	friend class ClassifBPlusTree;
	friend class InnerNode;
	friend class LeafNode;
	friend class BPlusTreeIterator;
	friend class OcurrenceTree;

private:
	int number;
	int level;
	int occupiedSpace;
	int keyMount;
	Key* keys;

public:
	Node(int level);
	virtual ~Node();

	bool isLeaf();
	bool isOverflow(int);
	bool isUnderflow();
	bool elementsCanTransfer();

    int getKeysMount() const
    {
        return keyMount;
    }

    Key* getKeys() const
    {
        return keys;
    }

    int getOccupiedSpace() const
    {
        return occupiedSpace;
    }

    int getLevel() const
    {
        return level;
    }

    int getNumber() const
    {
        return number;
    }

};

#endif /* NODE_H_ */
