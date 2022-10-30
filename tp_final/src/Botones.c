#include "Botones.h"

void prevSgn(uint8_t *sgnActual,GPDMA_LLI_Type *listaDma,uint16_t signals[][TRANSFERSIZE]){
	(*sgnActual)--;
	if(*sgnActual<0){
		*sgnActual=CANTIDADSGNLS-1;
	}
	changeSgn(*sgnActual,listaDma,signals);
}

void nextSgn(uint8_t *sgnActual,GPDMA_LLI_Type *listaDma,uint16_t signals[][TRANSFERSIZE]){
	(*sgnActual)++;
	if(*sgnActual==CANTIDADSGNLS){
		*sgnActual=0;
	}
	changeSgn(*sgnActual,listaDma,signals);
}

void static changeSgn(uint8_t sgnActual,GPDMA_LLI_Type *listaDma,uint16_t signals[][TRANSFERSIZE]){
	listaDma->SrcAddr = (uint32_t)signals[sgnActual];
}


void disminuirOct(uint8_t *octActual, uint16_t notas[]){
	if(*octActual>=0){
		(octActual)--;
		for(int i = 0;i<13;i++){
			notas[i]/=2;
		}
	}
}

void aumentarOct(uint8_t *octActual, uint16_t notas[]){
	if(*octActual<8){
		(octActual)++;
		for(int i = 0;i<13;i++){
			notas[i]*=2;
		}
	}
}


