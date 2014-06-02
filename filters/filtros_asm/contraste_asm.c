
#include "../../shared/px.h"
#include "../../shared/funciones.h"


void aplicarContraste_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int nivel);


IplImage * aplicar_filtro_contraste_asm(IplImage *src, void *param) {
	int nivel = *((int*)param);	//nivel entre -100 y 100

	IplImage *res;
	res = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	aplicarContraste_asm((unsigned char*)src->imageData, (unsigned char*)res->imageData, src->height, src->width, src->widthStep, res->widthStep, nivel);
	
	return res;
}
