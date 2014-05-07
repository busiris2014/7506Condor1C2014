
#include "InnerNode.h"


InnerNode::InnerNode(int nivel) : Node(nivel) {
	this->sons = new int[(TreeConstraits::getEfectiveSizeNode()/(TreeConstraits::getControlSizeRecord())) + 2];
}

InnerNode::~InnerNode() {
	delete[] sons;
}

ByteString InnerNode::Serialize() {
	ByteString byteStr;

	byteStr.insertLast(&level,sizeof(level));
	byteStr.insertLast(&keyMount, sizeof(keyMount));

	int position;
	for (position = 0; position < keyMount; ++position) {
		byteStr.insertLast(keys[position].Serialize());
		byteStr.insertLast(&sons[position], sizeof(int));
	}
	byteStr.insertLast(&sons[position], sizeof(int));

	char vacio = CONST_EMPTY;
	int freeSpace = TreeConstraits::getFullSizeNode() - byteStr.getSize();
	for (int i = 0; i < freeSpace; i++) {
		byteStr.insertLast(&vacio, sizeof(vacio));
	}
	return byteStr;
}

bool InnerNode::Hidratate(ByteString & byteStr) {
	bool success = true;

	if ((int)byteStr.getSize() != (TreeConstraits::getFullSizeNode())) {
		success = false;
	} else {
		int offset = 0;
		level = byteStr.readAsInt(offset);
		offset += sizeof(level);
		keyMount = byteStr.readAsInt(offset);
		offset += sizeof(keyMount);

		int position;
		for (position = 0; position < keyMount; ++position) {

			char keySize;
			byteStr.read(&keySize,offset,TAM_LONG_CLAVE);
			ByteString byteStrKey = byteStr.read(offset, TAM_LONG_CLAVE + keySize);
			Key aKey;
			aKey.Hidratate(byteStrKey);
			keys[position] = aKey;
			offset += TAM_LONG_CLAVE + keySize;

			sons[position] = byteStr.readAsInt(offset);
			offset += sizeof(int);
			occupiedSpace += keySize + TreeConstraits::getControlSizeRecord();
		}
		sons[position] = byteStr.readAsInt(offset);
	}
	return success;
}
