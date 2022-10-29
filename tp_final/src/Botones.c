#include "Botones.h"

void prevSgn(uint8_t *sgnActual){
	*sgnActual--;
	if(*sgnActual<0){
		*sgnActual=CANTIDADSGNLS-1;
	}
}

void nextSgn(uint8_t *sgnActual){
	*sgnActual++;
	if(*sgnActual==CANTIDADSGNLS){
		*sgnActual=0;
	}
}

void changeSgn(uint8_t *sgnActual,GPDMA_LLI_Type *listaDma,uint16_t signals[][TRANSFERSIZE]){
	listaDma->SrcAddr = signals[*sgnActual];
}


void disminuirOct(uint8_t *octActual, uint16_t notas[]){
	if(*octActual>=0){
		*octActual--;
		for(int i = 0;i<13;i++){
			notas[i]/=2;
		}
	}
}

void aumentarOct(uint8_t *octActual, uint16_t notas[]){
	if(*octActual<8){
		*octActual++;
		for(int i = 0;i<13;i++){
			notas[i]*=2;
		}
	}
}


