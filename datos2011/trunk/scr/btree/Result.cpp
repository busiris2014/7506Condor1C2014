
#include "Result.h"

Result::Result() {
	flags = OK;
}

Result::Result(eFlagsResultado flag) {
	flags = flag;
}

Result::Result(eFlagsResultado flag, Key k) {
	flags = flag;
	ultimaClave = k;
}

bool Result::contains(eFlagsResultado flag) const {
	return (flags & flag) != 0;
}

Result& Result::operator|=(const Result &aResult) {

	flags = eFlagsResultado(flags | aResult.flags);

	if (aResult.contains(ACTUALIZAR_ULTIMA_CLAVE))
		ultimaClave = aResult.ultimaClave;

	return *this;
}
