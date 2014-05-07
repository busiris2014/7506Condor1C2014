#ifndef GAMMACODER_H_
#define GAMMACODER_H_

#include <string>
#include "string.h"
#include <cmath>
#include "malloc.h"
#include <iostream>

using namespace std;

class gammaCoder {

private:
	short int remainingbits;
public:
	int nbytesallocked;
	gammaCoder();
	virtual ~gammaCoder();
	void reset();
	void encode(unsigned int toEncode,int &bits,void **coded);
	short int decode(void *toDecode,unsigned int &decoded);
};

#endif /* GAMMACODER_H_ */
