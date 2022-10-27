#include "Stack.h"
#include <stdlib.h>

Stack CreateStack()
{
    Stack stack = {.size = 0, .ultima = NULL};

    return stack;
}

int GetNumTecla(Stack *stack)
{
    if (stack->size == 0)
    {
        return -1;
    }

    return stack->ultima->numTecla;
}

void InsertTecla(Stack *stack, int numTecla)
{
    // Crear ultima tecla tocada
    Tecla *tecla = malloc(sizeof(Tecla));
    tecla->siguiente = NULL;
    tecla->numTecla = numTecla;

    // Linkear penultima tecla con ultima
    if (stack->size > 0)
    {
        tecla->anterior = stack->ultima;
        stack->ultima->siguiente = tecla;
    }

    // Poner tecla como ultima
    stack->ultima = tecla;

    stack->size++;
}

void RemoveTecla(Stack *stack, int numTecla)
{
    for (Tecla *tecla = stack->ultima; tecla != NULL; tecla = tecla->anterior)
    {
        if (tecla->numTecla == numTecla)
        {
            // Ultima tecla
            if (tecla == stack->ultima)
            {
                if (tecla->anterior != NULL)
                    tecla->anterior->siguiente = NULL;
                stack->ultima = tecla->anterior;
            }
            // primera tecla
            else if (tecla->anterior == NULL)
            {
                tecla->siguiente->anterior = NULL;
            }
            // Tecla en el medio
            else
            {
                tecla->anterior->siguiente = tecla->siguiente;
                tecla->siguiente->anterior = tecla->anterior;
            }

            free(tecla);

            stack->size--;

            return;
        }
    }
}
