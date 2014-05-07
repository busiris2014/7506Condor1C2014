#include "FrontCoder.h"

FrontCoder::FrontCoder() {
	this->lastString='\0';

}

FrontCoder::~FrontCoder() {
}

string FrontCoder::intToTwoDigitsString(short int number)
{
	char result[2];
	string strresult;
	if(number < 10)
		sprintf(result,"%d%d",0,number);
	else
		sprintf(result,"%d",number);
	strresult.append(result);
	return result;
}

short int FrontCoder::countEquals(string key)
{
	short int counter=0;
	while(this->lastString[counter] == key[counter])
	{
		counter++;
	}
	return counter;
}

string FrontCoder::recoverEquals(string key)
{
	string equals;
	short int number = atoi(&key.substr(0,2)[0]);
	for(int i=0;i<number;i++)
		equals.append(this->lastString.substr(i,1));
	return equals;
}

string FrontCoder::encode(string key)
{
	string result;
	string number;
	int equals=countEquals(key);
	number=this->intToTwoDigitsString(equals);
	result.append(number);
	number=this->intToTwoDigitsString(key.length()-equals);
	result.append(number);
	result.append(key.substr(equals,atoi(&number[0])));
	this->lastString=key;
	return result;
}

string FrontCoder::decode(string key)
{
	string result;
	result = this->recoverEquals(key);
	result.append(key.substr(4,atoi(&key.substr(2,2)[0])));
	this->lastString=result;
	return result;
}
