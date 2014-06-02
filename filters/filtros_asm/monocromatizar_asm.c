#include "cv.h" 

#ifndef MONOCROMATIZAR_ASM
#define MONOCROMATIZAR_ASM


void monocromatizar_inf_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size);



IplImage * aplicar_filtro_monocromatizar(IplImage *src, void *param) {
	IplImage *out;
	out = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	typedef void (monocromatizar_inf_fn_t) (unsigned char*, unsigned char*, int, int, int, int);
	monocromatizar_inf_fn_t *proceso;
	proceso = monocromatizar_inf_asm;

	proceso((unsigned char*)src->imageData, (unsigned char*)(out->imageData), src->height, src->width, src->widthStep, out->widthStep);

	return out;
}

#endif
