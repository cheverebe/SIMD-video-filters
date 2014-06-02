
#include "../../shared/px.h"
#include "../../shared/funciones.h"


void modificar_brillo_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int offset);


IplImage * aplicar_filtro_brillo_asm(IplImage *src, void *param) {
	int nivel = *((int*)param);
	int offset = (int)(((255+255) / 100) * nivel - 255);

	IplImage *res;
	res = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	
	modificar_brillo_asm((unsigned char*)src->imageData, (unsigned char*)res->imageData, src->height, src->width, src->widthStep, res->widthStep, offset);
	
	return res;
}
