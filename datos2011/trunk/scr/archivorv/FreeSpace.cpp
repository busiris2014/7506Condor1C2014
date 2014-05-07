/*
 * FreeSpace.cpp
 *
 *  Created on: 30/03/2011
 *      Author: luciano
 */

#include "FreeSpace.h"

FreeSpace::FreeSpace() {
	this->space = 0;
	this->filePosition = -1;
}

FreeSpace::FreeSpace(int position, int space) {
	this->space = space;
	this->filePosition = position;
}


FreeSpace::~FreeSpace() {
	// TODO Auto-generated destructor stub
}

int FreeSpace::getFreeSpace()
{
	return this->space;
}
int FreeSpace::getFilePosition()
{
	return this->filePosition;
}
void FreeSpace::setFreeSpace(int space)
{
	this->space = space;
}
void FreeSpace::setFilePosition(int position)
{
	this->filePosition = position;
}
