#include "../../shared/px.h"
#include "../../shared/funciones.h"
#include "monocromatizar_asm.c"

#ifndef BORDE_ASM
#define BORDE_ASM

void borde_lap_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);


IplImage * aplicar_filtro_borde_asm(IplImage * src, void *param) {
	
	IplImage *temp;
	temp = aplicar_filtro_monocromatizar(src, NULL);
	
	IplImage *res;
	res = cvCreateImage(cvGetSize(temp), IPL_DEPTH_8U, 1);
	
	borde_lap_asm((unsigned char *)(temp->imageData), (unsigned char *)(res->imageData), temp->height, temp->width, temp->widthStep, res->widthStep);
	
	cvReleaseImage(&temp);
	
	return res;
}

#endif
