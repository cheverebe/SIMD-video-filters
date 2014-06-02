#include "cv.h" 

void noise_asm(unsigned char* src_one, unsigned char* src_two, unsigned char* noise,int height, int width, int src_row_size );

IplImage * aplicar_filtro_noise_asm(IplImage *src, void *param) {
    IplImage *out;
    IplImage *srcPrev;
    
    IplImage **prev = (IplImage**)param;
    
    srcPrev = cvCloneImage(*prev);

    out = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

    typedef void (noise_fn_t) (unsigned char*, unsigned char*, unsigned char*,int, int, int );

    noise_fn_t *proceso;
    proceso = noise_asm;

    proceso((unsigned char*)src->imageData,(unsigned char*)srcPrev->imageData, (unsigned char*)(out->imageData), src->height, src->width, src->widthStep);
    
    cvReleaseImage(&srcPrev);

    return out;

}
