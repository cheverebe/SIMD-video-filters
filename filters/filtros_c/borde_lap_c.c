#ifndef BORDES_C
#define BORDES_C

#include "cv.h"


void borde_lap_c (unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);


IplImage * aplicar_filtro_borde_c(IplImage * src, void *param) {
	
	IplImage *temp;
	temp = aplicar_filtro_monocromatizar_c(src, NULL);
	
	IplImage *res;
	res = cvCreateImage(cvGetSize(temp), IPL_DEPTH_8U, 1);
	
	borde_lap_c((unsigned char *)(temp->imageData), (unsigned char *)(res->imageData), temp->height, temp->width, temp->widthStep, res->widthStep);
	
	cvReleaseImage(&temp);
	
	return res;
}


void borde_lap_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
	
	int laplace[3][3] = {{1,2,1},{2,-12,2},{1,2,1}};
	int valor = 0;
	
	dst += 1 + dst_row_size;
	
	unsigned char *base = src;
	unsigned char *pixel = src;
	
	for (int i = 1; i < (h-1); i++) {
		for (int j = 1; j < (w-1); j++) {
			valor = 0;
			for (int k = 0; k < 3; k++) {
				for (int l = 0; l < 3; l++) {
					pixel = base + (j+k-1) + (i+l-1) * src_row_size;
					valor += *pixel * laplace[k][l];
				}
			}
			valor = valor / 2;
			*(dst++) =  (valor < 0) ? 0: (valor > 255) ? 255: valor;
			src ++;
		}
		
		src += 2 + src_row_size - w;
		dst += 2 + dst_row_size - w;
	}		
}

#endif
