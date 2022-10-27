#include "Tecla.h"

typedef struct Stack
{
    Tecla *ultima;
    int size = 0;
} Stack;

int GetNumTecla(Stack *stack);
void InsertTecla(Stack *stack, int numTecla);
void RemoveTecla(Stack *stack, int numTecla);
