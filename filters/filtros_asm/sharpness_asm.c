#include "cv.h"


#ifndef SHARPNESS_ASM
#define SHARPNESS_ASM


#include "unir_canales_asm.h"


IplImage * sharpness_rgb_asm(IplImage *src);
void sharpness_un_canal_asm(unsigned char *src, unsigned char *dst, int h, int w, int row_size);


IplImage * aplicar_filtro_sharpness_asm(IplImage *src, void *param) {

	IplImage *res;
	res = sharpness_rgb_asm(src);
	return res;
}



IplImage * sharpness_rgb_asm(IplImage *src) {

	IplImage *outR;
	outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outG;
	outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outB;
	outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	separar_canales_asm((unsigned char*)src->imageData, (unsigned char*)(outR->imageData), (unsigned char*)(outG->imageData), (unsigned char*)(outB->imageData), src->height, src->width, src->widthStep, outR->widthStep);

	IplImage *out2R;
	IplImage *out2G;
	IplImage *out2B;

//este ciclo se usa para los tests de rendimiento

	//R
	out2R = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    sharpness_un_canal_asm((unsigned char*)outR->imageData, (unsigned char*)(out2R->imageData), src->height, src->width, outR->widthStep);
    
	//G
	out2G = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	sharpness_un_canal_asm((unsigned char*)outG->imageData, (unsigned char*)(out2G->imageData), src->height, src->width, outR->widthStep);
	
	//B
	out2B = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	sharpness_un_canal_asm((unsigned char*)outB->imageData, (unsigned char*)(out2B->imageData), src->height, src->width, outR->widthStep);

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
#endif
