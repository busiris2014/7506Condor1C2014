
#include "LeafNode.h"


LeafNode::LeafNode() : Node(0) {
	nextLeaf = 0;
	this->byteData = new ByteString[(TreeConstraits::getEfectiveSizeNode()/(TreeConstraits::getControlSizeRecord())) + 2];
}

LeafNode::~LeafNode() {
	delete[] byteData;
}

ByteString LeafNode::Serialize() {
	ByteString byteStr;

	FrontCoder encoder;

	byteStr.insertLast(&level, sizeof(level)); // 4 bytes level
	byteStr.insertLast(&keyMount, sizeof(keyMount)); // 4 bytes cant. records
	for (int pos = 0; pos < keyMount; ++pos) {
		byteStr.insertLast(keys[pos].SerializeEncoded(encoder));
		//byteStr.insertLast(keys[pos].Serialize());
		int dataSize = byteData[pos].getSize();
		byteStr.insertLast(&dataSize,sizeof(dataSize));
		byteStr.insertLast(byteData[pos]);
	}
	byteStr.insertLast(&nextLeaf, sizeof(nextLeaf));

	char empty = CONST_EMPTY;
	int freeSpace = TreeConstraits::getFullSizeNode() - byteStr.getSize();
	for (int i = 0; i < freeSpace; i++) {
		byteStr.insertLast(&empty, sizeof(empty));
	}
	return byteStr;
}

bool LeafNode::Hidratate(ByteString & byteStr) {
	bool success = true;

	if ((int)byteStr.getSize() != (TreeConstraits::getFullSizeNode())) {
		success = false;
	} else {
		int offset = 0;
		level = byteStr.readAsInt(offset);
		offset += sizeof(level);
		keyMount = byteStr.readAsInt(offset);
		offset += sizeof(keyMount);

		FrontCoder decoder;

		for (int position = 0; position < keyMount; ++position) {

			char keySize; // FIXME esto tendria que ser un int guardado en 4 bytes
			byteStr.read(&keySize,offset,TAM_LONG_CLAVE);
			ByteString byteStrKey = byteStr.read(offset, TAM_LONG_CLAVE + keySize);
			Key aKey;
			aKey.HidratateDecoded(byteStrKey,decoder);
			//aKey.Hidratate(byteStrKey);
			keys[position] = aKey;
			offset += TAM_LONG_CLAVE + keySize;
//			std::cout << offset << std::endl;
			int dataSize = byteStr.readAsInt(offset);
			offset += TAM_LONG_DATO;
			ByteString byteStrData;
			byteStrData.insertLast(byteStr.read(offset, dataSize));
			byteData[position] = byteStrData;

			offset += dataSize;
			occupiedSpace += keySize + dataSize + TreeConstraits::getControlSizeRecord();
		}
		nextLeaf = byteStr.readAsInt(offset);
	}
	return success;
}

LeafNode* LeafNode::clone() {
	LeafNode* leafNode = new LeafNode();
	leafNode->level = this->level;
	leafNode->occupiedSpace = this->occupiedSpace;
	leafNode->keyMount = this->keyMount;
	leafNode->nextLeaf = this->nextLeaf;
	for (int i=0; i < this->keyMount; ++i) {
		leafNode->keys[i] = this->keys[i];
	}
	for (int i = 0; i < this->keyMount; ++i) {
		leafNode->byteData[i] = this->byteData[i];
	}
	return leafNode;
}
