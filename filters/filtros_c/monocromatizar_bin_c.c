#include "cv.h"
#include "monocromatizar_inf_c.c"

#define UMBRAL 127

void monocromatizar_bin_c (unsigned char *src, unsigned char *dst, int h, int w, int row_size, unsigned char umbral);



IplImage * aplicar_filtro_monocromatizar_bin_c(IplImage * src, void *param) {

	IplImage *res;
	res = aplicar_filtro_monocromatizar_c(src, NULL);
	
	monocromatizar_bin_c((unsigned char *)(res->imageData), (unsigned char *)(res->imageData), res->height, res->width, res->widthStep, UMBRAL);
	
	return res;
}



void monocromatizar_bin_c (unsigned char *src, unsigned char *dst, int h, int w, int row_size, unsigned char umbral) {

	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			if (*src <= umbral) {
				*dst = 0;
			} else {
				*dst = 0xff;
			}
			src++;
			dst++;
		}
		src += row_size - w;
		dst += row_size - w;
	}

}
