#include "cv.h"


void separar(IplImage *src, IplImage **outR, IplImage **outG, IplImage **outB);


IplImage * aplicar_filtro_red(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	separar(src, &outR, &outG, &outB);

    cvReleaseImage(&outG);
    cvReleaseImage(&outB);
    
    return outR;
}


IplImage * aplicar_filtro_green(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	separar(src, &outR, &outG, &outB);

    cvReleaseImage(&outR);
    cvReleaseImage(&outB);
    
    return outG;
}


IplImage * aplicar_filtro_blue(IplImage *src, void *param) {
	IplImage * outR = 0;
	IplImage * outG = 0;
	IplImage * outB = 0;
	
	separar(src, &outR, &outG, &outB);

    cvReleaseImage(&outR);
    cvReleaseImage(&outG);
    
    return outB;
}

void separar(IplImage *src, IplImage **outR, IplImage **outG, IplImage **outB) {

    *outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    *outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    *outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

    typedef void (separar_canales_fn_t) (unsigned char*, unsigned char*, unsigned char*, unsigned char*, int, int, int, int);
    separar_canales_fn_t *proceso;
    proceso = separar_canales_asm;

    proceso((unsigned char*)src->imageData, (unsigned char*)((*outR)->imageData), 
    		(unsigned char*)((*outG)->imageData), (unsigned char*)((*outB)->imageData), 
    		src->height, src->width, src->widthStep, (*outR)->widthStep);

}
