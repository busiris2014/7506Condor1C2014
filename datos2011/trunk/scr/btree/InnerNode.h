
#ifndef INNERNODE_H_
#define INNERNODE_H_

#include "Node.h"
#include "TreeConstraits.h"


class InnerNode : public Node {

	friend class ClassifBPlusTree;
	friend class BPlusTree;
	friend class OcurrenceTree;

private:
	int* sons;

public:
	InnerNode(int level);
	virtual ~InnerNode();

	ByteString Serialize();
	bool Hidratate(ByteString & byteData);

    int* getSons() const
    {
        return sons;
    }
};

#endif /* INNERNODE_H_ */
