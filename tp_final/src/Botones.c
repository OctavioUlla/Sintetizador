#include "Botones.h"

void prevSgn(uint8_t *sgnActual){
	if((*sgnActual) == 0){
		(*sgnActual) = CANTIDADSGNLS-1;
	}else (*sgnActual)--;
}
// Funcion que pasa a la proxima señal
void nextSgn(uint8_t *sgnActual){
	if((*sgnActual) == (CANTIDADSGNLS-1)){
		(*sgnActual)=0;
	}else (*sgnActual)++;
}
// Funcion que disminuye octavas
void disminuirOct(uint8_t *octActual){
	if((*octActual)>0){
		(*octActual)--;
	}
}
// Funcion que aumenta octavas
void aumentarOct(uint8_t *octActual){
	if((*octActual)<8){
		(*octActual)++;
	}
}

