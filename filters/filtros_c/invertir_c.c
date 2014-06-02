#include "invertir_c.h"
#include "separar_canales_c.h"
#include "unir_canales_c.h"

#ifndef INVERTIR_C
#define INVERTIR_C

void invertir_c (unsigned char *src, unsigned char *dst, int h, int w, int row_size);

IplImage * aplicar_filtro_invertir_c(IplImage *src, void *param) {

    IplImage *final;
    final = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

    invertir_c((unsigned char*)src->imageData, (unsigned char*)(final->imageData), src->height, src->width, final->widthStep);

    return final;
}

void invertir_c(unsigned char *src, unsigned char *dst, int h, int w, int row_size) {
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w*3; i++) {
			*dst = 0xff - *src;
			dst++;
			src++;
		}
		dst += row_size - w*3;
		src += row_size - w*3;
	}
}

#endif
