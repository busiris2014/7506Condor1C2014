#ifndef COMPARERRECORD_H_
#define COMPARERRECORD_H_
#include "Comparer.h"
#include "Record.h"
class ComparerRecord: public Comparer<Record*> {

	/*
	 * Los registros se ordenan por key. Si dos registros tienen
	 * igual key se retorna 0.
	 * Si la key del regisotro A es mayor a la de B retorna -1
	 * En caso contrario retorna 1
	 */
	int compare(Record* A, Record* B);

};

#endif /* COMPARERRECORD_H_ */
