#include "cv.h"

void aplicar_filtro_separar_c(IplImage *src, IplImage **outR, IplImage **outG, IplImage **outB);
IplImage * aplicar_filtro_red_c(IplImage *src, void *param);
IplImage * aplicar_filtro_green_c(IplImage *src, void *param);
IplImage * aplicar_filtro_blue_c(IplImage *src, void *param);
