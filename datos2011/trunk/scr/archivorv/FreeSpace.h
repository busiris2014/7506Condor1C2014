/*
 * FreeSpace.h
 *
 *  Created on: 30/03/2011
 *      Author: luciano
 */

#ifndef FREESPACE_H_
#define FREESPACE_H_


class FreeSpace {
private:
	int filePosition;
	int space;
public:
	FreeSpace();
	FreeSpace(int posicion, int space);
	int getFreeSpace();
	int getFilePosition();
	void setFreeSpace(int space);
	void setFilePosition(int position);
	virtual ~FreeSpace();
};

#endif /* FREESPACE_H_ */
