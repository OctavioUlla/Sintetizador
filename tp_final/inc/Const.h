#ifndef CONST_H_
#define CONST_H_

#define TRANSFERSIZE  ((256))
#define DACSIZE  ((1024))
#define CANTIDADSGNLS  ((3))

typedef enum sgnls{
	SGNRECT = 0,
	SGNTRIANG = 1,
	SGNSIERRA = 2,
}SIGNAL_TYPE;

#endif /* CONST_H_ */
