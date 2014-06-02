#include "brillo_c.h"

#include "../../shared/px.h"
#include "../../shared/funciones.h"


void hacerDiff(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int offset);


IplImage * aplicar_filtro_brillo_c(IplImage *src, void *param) {
	int nivel = *((int*)param);
	int offset = (int)(((255+255) / 100) * nivel - 255);

	IplImage *res;
	res = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	
	hacerDiff((unsigned char*)src->imageData, (unsigned char*)res->imageData, src->height, src->width, src->widthStep, res->widthStep, offset);
	
	return res;
}

void hacerDiff(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int offset) {
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w*3; i++) {
			pixelRGB * px_src = (pixelRGB *)src;
			pixelRGB * px_dst = (pixelRGB *)dst;
			
			px_dst->R = max(0, min(px_src->R + offset, 255));
			px_dst->G = max(0, min(px_src->G + offset, 255));
			px_dst->B = max(0, min(px_src->B + offset, 255));
			
			dst++;
			src++;
		}
		dst += dst_row_size - w*3;
		src += src_row_size - w*3;
	}
}
