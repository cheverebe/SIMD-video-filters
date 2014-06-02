#include "cv.h" 
#include "unir_canales_asm.h"

void invertir_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size);


IplImage * aplicar_filtro_invertir(IplImage *src, void *param) {
	IplImage *final;
    final = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	
	invertir_asm((unsigned char*)src->imageData, (unsigned char*)(final->imageData), src->height, src->width, final->widthStep);

    return final;

}
