#include "cv.h"
#include "monocromatizar_asm.c"

#define UMBRAL 127

void monocromatizar_bin_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int umbral);



IplImage * aplicar_filtro_monocromatizar_bin_asm(IplImage * src, void *param) {

	IplImage *res;
	res = aplicar_filtro_monocromatizar(src, NULL);
	
	monocromatizar_bin_asm((unsigned char *)(res->imageData), (unsigned char *)(res->imageData), res->height, res->width, res->widthStep, UMBRAL);
	
	return res;
}
