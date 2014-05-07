
#include "Node.h"


Node::Node(int aLevel) {
	this->level = aLevel;
	this->occupiedSpace = 0;
	this->keyMount = 0;
	this->keys = new Key[(TreeConstraits::getEfectiveSizeNode()/(TreeConstraits::getControlSizeRecord())) + 2];
}

Node::~Node() {
	delete[] keys;
}

bool Node::isLeaf() {
	return (level == 0);
}

bool Node::isOverflow(int recordSize) {
	return (occupiedSpace + recordSize > TreeConstraits::getEfectiveSizeNode());
}

bool Node::isUnderflow() {
	return (occupiedSpace < TreeConstraits::getEfectiveSizeNode() / 2);
}

bool Node::elementsCanTransfer() {
	return (occupiedSpace > TreeConstraits::getEfectiveSizeNode() / 2);
}
