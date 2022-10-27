#include "Stack.h"
#include <stdlib.h>

Stack CreateStack()
{
    Stack stack = {.size = 0, .ultima = NULL};

    return stack;
}

int GetNumTecla(Stack *stack)
{
    return stack->ultima->numTecla;
}

void InsertTecla(Stack *stack, int numTecla)
{
    // Crear ultima tecla tocada
    Tecla tecla = {.anterior = stack->ultima, .numTecla = numTecla};

    // Poner tecla como ultima
    stack->ultima = &tecla;
}

void RemoveTecla(Stack *stack, int numTecla)
{
    for (int i = 0; i < stack->size; i++)
    {
        /* code */
    }
}