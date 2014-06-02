#ifndef SUAVIZAR_C_RGB
#define SUAVIZAR_C_RGB

#include "cv.h"

#include "suavizar_c.c"
#include "separar_canales_c.h"


void separar_canales_c(unsigned char *src, unsigned char *dst_r, unsigned char *dst_g, unsigned char *dst_b, int h, int w, int src_row_size, int dst_row_size);

void unir_canales_c(unsigned char *dst, unsigned char *src_r, unsigned char *src_g, unsigned char *src_b, int height, int width, int src_row_size, int dst_row_size );


IplImage * aplicar_filtro_suavizado_c_rgb(IplImage *src, void *param) {

	IplImage *outR;
	outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outG;
	outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outB;
	outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	typedef void (separar_canales_fn_t) (unsigned char*, unsigned char*, unsigned char*, unsigned char*, int, int, int, int);
	separar_canales_fn_t *proceso;
	proceso = separar_canales_c;

	proceso((unsigned char*)src->imageData, (unsigned char*)(outR->imageData), (unsigned char*)(outG->imageData), (unsigned char*)(outB->imageData), src->height, src->width, src->widthStep, outR->widthStep);

	//SUAVIZO

	//R
	IplImage *out2R;
	out2R = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    suavizar_c((unsigned char*)outR->imageData, (unsigned char*)(out2R->imageData), src->height, src->width, outR->widthStep);

	//G
	IplImage *out2G;
	out2G = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	suavizar_c((unsigned char*)outG->imageData, (unsigned char*)(out2G->imageData), src->height, src->width, outR->widthStep);
	
	//B
	IplImage *out2B;
	out2B = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	suavizar_c((unsigned char*)outB->imageData, (unsigned char*)(out2B->imageData), src->height, src->width, outR->widthStep);
	
    //ELIMINO LAS IMAGENES
    cvReleaseImage(&outR);
    cvReleaseImage(&outG);
    cvReleaseImage(&outB);

	//JUNTO
	IplImage *final;
    final = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
		
	typedef void (unir_canales_fn_t) (unsigned char*, unsigned char*, unsigned char*, unsigned char*, int, int, int, int);
	unir_canales_fn_t *unir;
	unir = unir_canales_c;
	
    unir((unsigned char*)final->imageData, (unsigned char*)(out2R->imageData), (unsigned char*)(out2G->imageData), (unsigned char*)(out2B->imageData), src->height, src->width, out2R->widthStep, src->widthStep);

    //ELIMINO LAS IMAGENES
    cvReleaseImage(&out2R);
    cvReleaseImage(&out2G);
    cvReleaseImage(&out2B);

    return final;

}

#endif
