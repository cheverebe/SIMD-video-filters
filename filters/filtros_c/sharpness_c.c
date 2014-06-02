#include "cv.h"


#ifndef SHARPNESS_C
#define SHARPNESS_C


IplImage * sharpness_rgb(IplImage *src);
void sharpness_un_canal(unsigned char *src, unsigned char *dst, int h, int w, int row_size);


IplImage * aplicar_filtro_sharpness(IplImage *src, void *param) {

	IplImage *res;
	res = sharpness_rgb(src);
	return res;
}



IplImage * sharpness_rgb(IplImage *src) {

	IplImage *outR;
	outR = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outG;
	outG = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	IplImage *outB;
	outB = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);

	separar_canales_c((unsigned char*)src->imageData, (unsigned char*)(outR->imageData), (unsigned char*)(outG->imageData), (unsigned char*)(outB->imageData), src->height, src->width, src->widthStep, outR->widthStep);
	
	IplImage *out2R;
	IplImage *out2G;
	IplImage *out2B;
		

	//R
	out2R = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
    sharpness_un_canal((unsigned char*)outR->imageData, (unsigned char*)(out2R->imageData), src->height, src->width, outR->widthStep);
    
	//G
	out2G = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	sharpness_un_canal((unsigned char*)outG->imageData, (unsigned char*)(out2G->imageData), src->height, src->width, outR->widthStep);
	
	//B
	out2B = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 1);
	sharpness_un_canal((unsigned char*)outB->imageData, (unsigned char*)(out2B->imageData), src->height, src->width, outR->widthStep);

    //ELIMINO LAS IMAGENES
    cvReleaseImage(&outR);
    cvReleaseImage(&outG);
    cvReleaseImage(&outB);

	//JUNTO
	IplImage *final;
    final = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);
	
    unir_canales_c((unsigned char*)final->imageData, (unsigned char*)(out2R->imageData), (unsigned char*)(out2G->imageData), (unsigned char*)(out2B->imageData), src->height, src->width, out2R->widthStep, src->widthStep);

    //ELIMINO LAS IMAGENES
    cvReleaseImage(&out2R);
    cvReleaseImage(&out2G);
    cvReleaseImage(&out2B);

    return final;
}


void sharpness_un_canal(unsigned char *src, unsigned char *dst, int h, int w, int row_size) {

	for (int j = 0; j < h; j++) 
	{
		for (int i = 0; i < w; i++) 
		{
			if (j == 0 || j == h - 1 || i == 0 || i == w - 1) 
			{
				*dst = *src;
			}
			else 
			{
				unsigned short result = *(src);
				unsigned short original = result;
				
				result += *(src - row_size) << 1;
				result += *(src - row_size + 1);
				result += *(src - 1) << 1;
				result += *(src) << 2;
				result += *(src + 1) << 1;
				result += *(src + row_size - 1);
				result += *(src + row_size) << 1;
				result += *(src + row_size + 1);
				
				int res_px = original*2 - (unsigned char)(result >> 4);
				
				if (res_px < 0)	res_px = 0;
				if (res_px > 0xff) res_px = 0xff;
				
				*dst = (unsigned short)res_px;
			}
			src++;
			dst++;
		}
		src += row_size - w;
		dst += row_size - w;
	}
}

#endif
