#include "Botones.h"

void prevSgn(uint8_t *sgnActual,uint32_t signals[][TRANSFERSIZE]){
	(*sgnActual)--;
	if((*sgnActual)+1==0){
		*sgnActual=CANTIDADSGNLS-1;
	}
}

void nextSgn(uint8_t *sgnActual,uint32_t signals[][TRANSFERSIZE]){
	(*sgnActual)++;
	if(*sgnActual==CANTIDADSGNLS){
		*sgnActual=0;
	}
}

void disminuirOct(uint8_t *octActual, uint16_t notas[]){
	if((*octActual)+1==0){
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


