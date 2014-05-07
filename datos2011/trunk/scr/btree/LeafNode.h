
#ifndef LEAFNODE_H_
#define LEAFNODE_H_

#include "Node.h"
#include "../common/ByteString.h"
#include "FrontCoder.h"
#include "TreeConstraits.h"


class LeafNode : public Node {

	friend class ClassifBPlusTree;
	friend class BPlusTree;
	friend class BPlusTreeIterator;
	friend class OcurrenceTree;

private:
	int nextLeaf;
	ByteString* byteData;

public:
	LeafNode();
	virtual ~LeafNode();

	ByteString Serialize();
	bool Hidratate(ByteString & cadena);

	ByteString *getDatos() const
    {
        return byteData;
    }

    int getHojaSiguiente() const
    {
        return nextLeaf;
    }

    LeafNode* clone();
};

#endif /* LEAFNODE_H_ */
