#include "cv.h" 

#include "../../filtros/filtros_asm/suavizar_asm.c"
#include "contarblancos_asm.h"

void diff_asm(unsigned char* src_one, unsigned char* src_two, unsigned char* diff,int height, int width, int src_row_size, int dst_row_size, int tolerance);


IplImage * aplicar_filtro_diff(IplImage *src, void *param) {
    IplImage *out;
    
	void** p = (void**)param;
    IplImage **prev = (IplImage**)(p[0]);
	int tolerancia = (int)(p[1]);
	int porcentajePxBlancos = (int)(p[2]);
	
	int cantPxBlancos = (640*480 * porcentajePxBlancos / 100);
    
	IplImage * srcSuavizado = aplicar_filtro_suavizar(src, NULL);
	IplImage * prevSuavizado = aplicar_filtro_suavizar(*prev, NULL);

    out = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    diff_asm((unsigned char*)srcSuavizado->imageData,(unsigned char*)prevSuavizado->imageData, (unsigned char*)(out->imageData), src->height, src->width, src->widthStep, out->widthStep, tolerancia);

	cvReleaseImage(&srcSuavizado);
	cvReleaseImage(&prevSuavizado);
	
	int numBlancos = 0;
	numBlancos = contarblancos_asm((unsigned char*)(out->imageData), out->height, out->width, out->widthStep);
	
	if (numBlancos > cantPxBlancos) {
		printf("Se detecto movimiento. Diff: %d\n", numBlancos);
	}

    return out;
}
