#include "Tecla.h"

typedef struct Stack
{
    Tecla *ultima;
    int size;
} Stack;

Stack CreateStack();
int GetNumTecla(Stack *stack);
void InsertTecla(Stack *stack, int numTecla);
void RemoveTecla(Stack *stack, int numTecla);
