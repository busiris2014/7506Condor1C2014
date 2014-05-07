/*
 * ClassifBPlusTree.h
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#ifndef CLASSIFBPLUSTREE_H_
#define CLASSIFBPLUSTREE_H_
#include "BPlusTree.h"
#include "FileBlockNodeMapper.h"
#include "../common/ListofID.h"

class ClassifBPlusTree : public BPlusTree{

	friend class Node;
	friend class InnerNode;
	friend class LeafNode;

private:
	FileBlockNodeMapper* fileBlockNodeMapper;
	bool addToListofId(int listNumber, int idToAdd);
	void addListToListofId(int listNumber, list<unsigned int> listToAdd);
public:
	ClassifBPlusTree(string filePath, int maxKeySize);
	void persistNode(Node* n);
	Node* hidratateNode(int nodeNumber);
	bool add(Record* record);
	bool removeKey(Key key,int idToRemove);
	void exportNode(ofstream& salida, Node* unNodo, int tabulacion,bool keytext, bool textContent);
	list<int>* searchAllIds(Key k);
	bool addList(Record* record,list<unsigned int> listToAdd);
	~ClassifBPlusTree();

};

#endif /* CLASSIFBPLUSTREE_H_ */
