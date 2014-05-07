/*
 * BlockNodeRecord.cpp
 *
 *  Created on: 21/04/2011
 *      Author: luciano
 */

#include "BlockNodeRecord.h"

BlockNodeRecord::BlockNodeRecord() {
	// TODO Auto-generated constructor stub

}

BlockNodeRecord::~BlockNodeRecord() {
	// TODO Auto-generated destructor stub
}

BlockNodeRecord::BlockNodeRecord(int node, int block)
{
	this->node = node;
	this->block = block;
}
int BlockNodeRecord::getNode()
{
	return this->node;
}
int BlockNodeRecord::getBlock()
{
	return this->block;
}
void BlockNodeRecord::setNode(int node)
{
	this->node = node;
}
void BlockNodeRecord::setBlock(int block)
{
	this->block = block;
}
