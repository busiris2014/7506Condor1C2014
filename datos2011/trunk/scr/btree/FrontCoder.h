#ifndef FRONTCODER_H_
#define FRONTCODER_H_

#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

class FrontCoder {

private:
	string lastString;
	string intToTwoDigitsString(short int number);
	short int countEquals(string key);
	string recoverEquals(string key);

public:
	FrontCoder();
	virtual ~FrontCoder();
	string encode(string key);
	string decode(string key);

};

#endif /* FRONTCODER_H_ */
