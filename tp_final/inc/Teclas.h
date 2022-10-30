#include "Tecla.h"
#include <stdint.h>

typedef struct Stack
{
    Tecla *ultima;
    int size;
} Stack;

Stack CreateStack();
int GetNumTecla(Stack *stack);
void InsertTecla(Stack *stack, int numTecla);
void RemoveTecla(Stack *stack, int numTecla);
void UpdateDMAFrecuency(Stack *stack,uint16_t *notas);
