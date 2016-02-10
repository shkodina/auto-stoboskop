#ifndef EXITDISP_H_2012_11_12
#define EXITDISP_H_2012_11_12

#include "makros.h"

#define TUG_ON 0xff
#define TUG_OFF 0x00
#define TUG_ALWAYS 0xffff
#define TUG_NEVER 0x0000
	
typedef struct Tugling_struct{
	uchar state;
	int ms10_period; 
} Tugling;

typedef struct Alg_struct{
	uchar len;
	uchar pos;
	Tugling * tugling_arr;
} Alg;

typedef struct SoftTimer_struct{
	uint val;
	uint cur;
	uchar is_elapsed;
	uint count;
} SoftTimer;


#endif
