#ifndef CANALES_C
#define CANALES_C

#include "separar_canales_c.h"



void separar_canales_c (unsigned char *src, unsigned char *dst_r, unsigned char *dst_g, unsigned char *dst_b, int h, int w, int src_row_size, int dst_row_size) {

	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
            *dst_b = *src;
            dst_b++;
            src++;

            *dst_g = *src;
            dst_g++;
			src++;

			*dst_r = *src;
            dst_r++;
			src++;
		}

		src += src_row_size - w*3;
		dst_r += dst_row_size - w;
		dst_g += dst_row_size - w;
		dst_b += dst_row_size - w;
	}

}


IplImage * aplicar_filtro_red_c(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	aplicar_filtro_separar_c(src, &outR, &outG, &outB);

    cvReleaseImage(&outG);
    cvReleaseImage(&outB);
    
    return outR;
}


IplImage * aplicar_filtro_green_c(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	aplicar_filtro_separar_c(src, &outR, &outG, &outB);

    cvReleaseImage(&outR);
    cvReleaseImage(&outB);
    
    return outG;
}


IplImage * aplicar_filtro_blue_c(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	aplicar_filtro_separar_c(src, &outR, &outG, &outB);

    cvReleaseImage(&outR);
    cvReleaseImage(&outG);
    
    return outB;
}

void aplicar_filtro_separar_c(IplImage *src, IplImage **outR, IplImage **outG, IplImage **outB) {

    *outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    *outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    *outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    separar_canales_c((unsigned char*)src->imageData, (unsigned char*)((*outR)->imageData), 
    		(unsigned char*)((*outG)->imageData), (unsigned char*)((*outB)->imageData), 
    		src->height, src->width, src->widthStep, (*outR)->widthStep);

}

#endif
