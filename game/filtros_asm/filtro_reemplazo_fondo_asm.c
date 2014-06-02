#include "cv.h"

void reemplazarFondo(unsigned char* src_one, unsigned char* src_two, unsigned char* back, unsigned char* dst, int height, int width, int src_row_size, int tolerance);


IplImage * aplicar_filtro_reemplazar_fondo(IplImage *src, void *param) {

    IplImage *out;
    out = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	void** p = (void**)param;
    IplImage **firstFrame = (IplImage**)(p[0]);
	int tolerancia = (int)(p[1]);
	
	IplImage *ss;
	ss = cvCloneImage(*firstFrame);

    IplImage *back;
    back = cvLoadImage("images/fondo.jpg", CV_LOAD_IMAGE_COLOR);
   
    reemplazarFondo((unsigned char*)src->imageData, (unsigned char*)ss->imageData, (unsigned char*)back->imageData, (unsigned char*)out->imageData, src->height, src->width, src->widthStep, tolerancia);

    cvReleaseImage(&ss);
    cvReleaseImage(&back);

    return out;
}
