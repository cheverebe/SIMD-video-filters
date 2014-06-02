#include "cv.h" 

#include "unir_canales_asm.h"

void separar_canales_asm(unsigned char *src, unsigned char *dst_r, unsigned char *dst_g, unsigned char *dst_b, int h, int w, int src_row_size, int dst_row_size);

void suavizar_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size);


	//SEPARO LOS CANALES
IplImage * aplicar_filtro_suavizar(IplImage *src, void *param) {

	IplImage *outR;
	outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outG;
	outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outB;
	outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	separar_canales_asm((unsigned char*)src->imageData, (unsigned char*)(outR->imageData), (unsigned char*)(outG->imageData), (unsigned char*)(outB->imageData), src->height, src->width, src->widthStep, outR->widthStep);

	//SUAVIZO
	//R
	IplImage *out2R;
	out2R = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    suavizar_asm((unsigned char*)outR->imageData, (unsigned char*)(out2R->imageData), src->height, src->width, outR->widthStep);
	
	//G
	IplImage *out2G;
	out2G = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	suavizar_asm((unsigned char*)outG->imageData, (unsigned char*)(out2G->imageData), src->height, src->width, outR->widthStep);
	
	//B
	IplImage *out2B;
	out2B = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	suavizar_asm((unsigned char*)outB->imageData, (unsigned char*)(out2B->imageData), src->height, src->width, outR->widthStep);
	
    //ELIMINO LAS IMAGENES
    cvReleaseImage(&outR);
    cvReleaseImage(&outG);
    cvReleaseImage(&outB);

	//JUNTO
	IplImage *final;
    final = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
			
    unir_canales_asm((unsigned char*)final->imageData, (unsigned char*)(out2R->imageData), (unsigned char*)(out2G->imageData), (unsigned char*)(out2B->imageData), src->height, src->width, out2R->widthStep, src->widthStep);

    //ELIMINO LAS IMAGENES
    cvReleaseImage(&out2R);
    cvReleaseImage(&out2G);
    cvReleaseImage(&out2B);

    return final;
}
