/*
 * BlockNodeRecord.h
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#ifndef BLOCKNODERECORD_H_
#define BLOCKNODERECORD_H_

class BlockNodeRecord {
	private:
	int node;
	int block;
public:
	BlockNodeRecord();
	BlockNodeRecord(int node, int block);
	int getNode();
	int getBlock();
	void setNode(int node);
	void setBlock(int block);
	virtual ~BlockNodeRecord();

};

#endif /* BLOCKNODERECORD_H_ */
