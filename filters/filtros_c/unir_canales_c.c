
#include "unir_canales_c.h"

#ifndef UNIR_CANALES_C
#define UNIR_CANALES_C

// B G R
void unir_canales_c(unsigned char *dst, unsigned char *src_r, unsigned char *src_g, unsigned char *src_b, int height, int width, int src_row_size, int dst_row_size ) {
	unsigned char red, blue, green;
	int f = 0;

	for (int j = 0; j < height; j++)	{
		for (int i = 0; i < 3*width; i+=3){
			f = i/3;
			red = src_r[f+src_row_size*j];
			green = src_g[f+src_row_size*j];
			blue = src_b[f+src_row_size*j];

			dst[i+dst_row_size*j] = blue;
			dst[i+1+dst_row_size*j] = green;
			dst[i+2+dst_row_size*j] = red;
		}
	}
}

#endif
