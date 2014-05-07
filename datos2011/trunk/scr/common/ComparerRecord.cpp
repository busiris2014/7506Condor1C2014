#include "ComparerRecord.h"

int ComparerRecord::compare(Record* A, Record* B){
	int resultado = 0;
	if(A->getKey()->getKey() > B->getKey()->getKey()){
		resultado = -1;
	}else{
		if(B->getKey()->getKey() > A->getKey()->getKey()){
			resultado = 1;
		}else{
			resultado = 0;
		}
	}
	return resultado;
}
