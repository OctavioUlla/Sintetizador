#include <stdlib.h>
#include "Teclas.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "Const.h"

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
    tecla->anterior = NULL;
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

void UpdateDMAFrecuency(Stack *stack,uint16_t *notas){

	DAC_CONVERTER_CFG_Type daccfg;

	int tecla = GetNumTecla(stack);

	if (tecla == -1){
		//GPDMA_ChannelCmd(0,DISABLE);
		daccfg.CNT_ENA = RESET; // Se habilita el timeout counter
		DAC_ConfigDAConverterControl(LPC_DAC,&daccfg);
		return;
	}

	//25MHz por clock del CPU 100MHz y transferSize
	uint32_t dmaCounter = (25 * 1000000)/(notas[tecla]*TRANSFERSIZE);
	DAC_SetDMATimeOut(LPC_DAC,dmaCounter);
	//GPDMA_ChannelCmd(0,ENABLE);
	daccfg.CNT_ENA = SET; // Se habilita el timeout counter
	DAC_ConfigDAConverterControl(LPC_DAC,&daccfg);
}
