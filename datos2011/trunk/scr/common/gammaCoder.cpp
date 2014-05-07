#include "gammaCoder.h"

gammaCoder::gammaCoder() {
	this->remainingbits=0;
	this->nbytesallocked=0;
}

gammaCoder::~gammaCoder() {

}

void gammaCoder::encode(unsigned int toEncode,int &bits,void **coded)
{
	int prev=0;
	if(this->remainingbits==0)
	{
		this->remainingbits=8;
		prev=1;
	}
	if(this->remainingbits<8) prev=1;

	unsigned int exp = (unsigned int) floor(log2(toEncode))+1;
	int r = toEncode - pow(2,exp-1);
	bits = 2*exp+1;

	unsigned int nbytes =(unsigned int) ceil((bits-this->remainingbits) / 8.0);
	if((this->remainingbits==8)&&(this->nbytesallocked!=0))nbytes+=1;



	if(this->nbytesallocked==0) this->nbytesallocked=1;
	void *result = malloc(this->nbytesallocked + nbytes);
	memset(result,0,this->nbytesallocked+nbytes);
	if(*coded != 0)
	{
		memcpy(result,*coded,this->nbytesallocked);

	}

	char c = 0;
	int bytesprocessed = 0;
	//if(this->remainingbits < 8) bytesprocessed=-1;
	for(int i=0;i<(int)exp;i++)
	{
		c=pow(2,this->remainingbits-1);
		*((unsigned char*)result+this->nbytesallocked+bytesprocessed-prev) = *((unsigned char*)result+this->nbytesallocked+bytesprocessed-prev) | c;
		if((this->remainingbits-1)==0)
		{
			bytesprocessed+=1;
			this->remainingbits=8;
		}
		else this->remainingbits--;
	}

	if((this->remainingbits-1)==0)
	{
		bytesprocessed+=1;
		this->remainingbits=8;
	}
	else this->remainingbits--;

	int *binaryarray = (int*) malloc(exp*sizeof(int));
	memset(binaryarray,0,exp*sizeof(int));
	int i = -1;
	int number=r;
	while(number != 0)
	{
		i++;
		binaryarray[i] = number%2;
		number=number/2;
	}

	c = 0;
	for(int j=0;j<exp;j++)
	{
		if(binaryarray[exp-1-j]==1)
		{
			c=pow(2,this->remainingbits-1);
			*((unsigned char*)result + this->nbytesallocked+bytesprocessed-prev) = *((unsigned char*)result+this->nbytesallocked + bytesprocessed-prev) | c;

		}
		if(this->remainingbits==1)
			{
				this->remainingbits=8;
				bytesprocessed++;
			}
		else this->remainingbits--;

	}

	//this->remainingbits=nbytes*8+this->remainingbits-bits;
	this->nbytesallocked = this->nbytesallocked + nbytes;
	if(*coded!=0)free(*coded);
	free(binaryarray);
	*coded = result;
}

short int gammaCoder::decode(void *toDecode,unsigned int &decoded)
{
	int exp = 0;
	int r=0;
	int bytesprocessed = 0;

	int startingbit=0;
	if(this->remainingbits==0) startingbit=7;
	else startingbit=this->remainingbits-1;

	char bitcomparator = pow(2,startingbit);

	while(bitcomparator != 0)
	{
		bitcomparator=pow(2,startingbit);
		bitcomparator = *((unsigned char*)toDecode+this->nbytesallocked+bytesprocessed) & bitcomparator;
		if(bitcomparator!=0) exp+=1;
		if((startingbit%8)==0) bytesprocessed++;
		if (startingbit==0) startingbit=7;
		else startingbit--;
	}
	int startingbit2=startingbit;

	for(int i =0;i<exp;i++)
	{
		bitcomparator=pow(2,startingbit2);
		bitcomparator=*((unsigned char*)toDecode+this->nbytesallocked+bytesprocessed) & bitcomparator;
		if(bitcomparator!=0)r=r+pow(2,exp-i-1);
		if((startingbit2%8)==0) bytesprocessed++;
		if (startingbit2==0) startingbit2=7;
		else startingbit2--;
	}


	decoded = pow(2,exp-1) + r;

	this->nbytesallocked = this->nbytesallocked+bytesprocessed;
	this->remainingbits=startingbit2+1;

	return 0;

}

void gammaCoder::reset()
{
	this->remainingbits=0;
	this->nbytesallocked=0;
}
