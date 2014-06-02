#include "contraste_c.h"

#include "../../shared/px.h"
#include "../../shared/funciones.h"


void aplicarContraste(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int offset);
unsigned char configurar_color(unsigned char color, float valor);


IplImage * aplicar_filtro_contraste_c(IplImage *src, void *param) {
	int nivel = *((int*)param);	//nivel entre -100 y 100

	IplImage *res;
	res = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	
	aplicarContraste((unsigned char*)src->imageData, (unsigned char*)res->imageData, src->height, src->width, src->widthStep, res->widthStep, nivel);
	
	return res;
}

void aplicarContraste(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size, int offset) {
	float valor = (offset + 100.0f) / 100.0f;
	valor *= valor;

	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w*3; i++) {
			pixelRGB * px_src = (pixelRGB *)src;
			pixelRGB * px_dst = (pixelRGB *)dst;
			
			px_dst->R = configurar_color(px_src->R, valor);
			px_dst->G = configurar_color(px_src->G, valor);
			px_dst->B = configurar_color(px_src->B, valor);
			
			dst++;
			src++;
		}
		dst += dst_row_size - w*3;
		src += src_row_size - w*3;
	}
}

unsigned char configurar_color(unsigned char color, float valor) {
	float ncolor = color / 255.0f;
	ncolor = (((ncolor - 0.5f) * valor) + 0.5f) * 255.0f;
	return (unsigned char)min(255, max(0, ncolor));
}
