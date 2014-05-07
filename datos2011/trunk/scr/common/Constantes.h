#define CONST_EMPTY 0x00
#define TAM_LONG_REGISTRO sizeof(int)
#define TAM_LONG_CLAVE sizeof(char)
#define TAM_LONG_DATO sizeof(int)

/***************** Arbol *******************/
// @DEPRECATED
#define TAM_TOTAL_NODO 4096//(BASE_TAMANIO<<POTENCIA)
#define TAM_CONTROL_NODO 12
#define TAM_EFECTIVO_NODO (TAM_TOTAL_NODO - TAM_CONTROL_NODO)
#define TAM_CONTROL_REGISTRO 5
#define PORC_TAMANIO_NODO 50
