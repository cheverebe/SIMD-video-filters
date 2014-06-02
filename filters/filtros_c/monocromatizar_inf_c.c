#ifndef MONO_C
#define MONO_C

#include "cv.h"

#include "../../shared/px.h"
#include "../../shared/funciones.h"


void realizarAccionInf(pixelRGB * src, pixelBW * dst);
void monocromatizar_inf_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);
IplImage * aplicar_filtro_monocromatizar_c(IplImage *src, void *param);


IplImage * aplicar_filtro_monocromatizar_c(IplImage *src, void *param) {
	IplImage *res;
	res = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	
	monocromatizar_inf_c((unsigned char *)(src->imageData), (unsigned char *)(res->imageData), src->height, src->width, src->widthStep, res->widthStep);
	
	return res;
}

void realizarAccionInf(pixelRGB * src, pixelBW * dst) {
	dst->C = max(max(src->R, src->G), src->B);
}

void monocromatizar_inf_c(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size) {
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			realizarAccionInf((pixelRGB *)src, (pixelBW *)dst);
			dst += 1;
			src += 3;
		}
		dst += dst_row_size - w*1;
		src += src_row_size - w*3;
	}
}

#endif
