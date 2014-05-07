
#ifndef RESULT_H_
#define RESULT_H_

#include "../common/Key.h"


class Result {

	friend class BPlusTree;

private:

	enum eFlagsResultado {OK = 0, NO_ENCONTRADO = 1, ACTUALIZAR_ULTIMA_CLAVE = 2, FUSION_NODOS = 4} flags;
	Key ultimaClave;

public:
	Result();
	Result(eFlagsResultado flag);
	Result(eFlagsResultado flag, Key k);
	bool contains(eFlagsResultado flag) const;
	Result& operator|=(const Result &aResult);

};

#endif /* RESULT_H_ */
