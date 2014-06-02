#include "cv.h" 

#define ARR_DER 1
#define ABA_DER 2
#define ARR_IZQ 3
#define ABA_IZQ 4

#define RADIO 10
#define CAR_Y_DEFAULT 50
#define CAR_RADIO 45
#define CAR_H 18
#define ANCHO 640
#define BORDE 15

    static int ultimaPosX=ANCHO/2;
	static int ultimaPosY=410;
    static int radioBola=RADIO;

    static int minY=0;
    static int minX=RADIO+10;
    static int maxY=410;
    static int maxX=ANCHO-10;

    static int direccion=ABA_DER;
    static IplImage *back;
    static IplImage *car;
    static IplImage *ball;


void hacerAuto_c (unsigned char* car, unsigned char* out,int pos,int height, int width, int src_row_size , int car_row_size );

int buscarPos_c (unsigned char* src_one, unsigned char* src_two,int height, int width, int src_row_size );

void hacerPelota_c (unsigned char* ball, unsigned char* out,int posX,int posY,int height, int width, int src_row_size , int ball_row_size );

IplImage * aplicar_filtro_juego(IplImage *src, void *param) {
    IplImage *srcPrev;

        void** p = (void**)param;
        IplImage **prev = (IplImage**)(p[0]);
        int tolerancia = (int)(p[1]);

        srcPrev = cvCloneImage(*prev);

    IplImage *out;
    if(maxY==0)
        maxY=src->height-radioBola;

    if(maxX==0)
        maxX=src->width-radioBola;

    IplImage *ss;
    ss=srcPrev;
    if(!back)
        back=cvLoadImage("images/image3.jpg",CV_LOAD_IMAGE_COLOR);
    if(!car)
        car=cvLoadImage("images/cosito.jpg",CV_LOAD_IMAGE_COLOR);
    if(!ball)
        ball=cvLoadImage("images/ball.png",CV_LOAD_IMAGE_COLOR);

    out = cvCloneImage( back );

    typedef int (buscarPos_fn_t) (unsigned char*, unsigned char*,int, int, int);
    buscarPos_fn_t *proceso;
    proceso = buscarPos_c;

    int pos=proceso((unsigned char*)src->imageData,(unsigned char*)ss->imageData,src->height, src->width, src->widthStep);
    pos=src->width-pos;
    if(pos<=CAR_RADIO+BORDE)
        pos=CAR_RADIO+BORDE;
    if(pos>=ANCHO-CAR_RADIO-BORDE)
        pos=ANCHO-CAR_RADIO-BORDE;

    typedef void (hacerAuto_fn_t) (unsigned char*, unsigned char*,int,int, int, int, int);
    hacerAuto_fn_t *proceso2;
    proceso2 = hacerAuto_c;
    proceso2((unsigned char*)car->imageData,(unsigned char*)out->imageData,pos,src->height, src->width, src->widthStep, car->widthStep);

    int posX=prox_ball_x();
    int posY=prox_ball_y(pos);

    typedef void (hacerPelota_fn_t) (unsigned char*, unsigned char*,int,int,int, int, int, int);
    hacerPelota_fn_t *proceso3;
    proceso3 = hacerPelota_c;
    proceso3((unsigned char*)ball->imageData,(unsigned char*)out->imageData,posX,posY,src->height, src->width, src->widthStep, ball->widthStep);

    cvReleaseImage(&ss);
    cvReleaseImage(&car);
    cvReleaseImage(&back);
    return out;

}
int prox_ball_x(){
	if(maxX==0){
		return ultimaPosX;
    }else
    if(ultimaPosY<=minY+radioBola && (direccion==ABA_IZQ||direccion==ABA_DER)){
        ultimaPosX=maxX/2;
        return ultimaPosX;
    }else
	if(ultimaPosX>=maxX-radioBola && (direccion==ARR_DER||direccion==ABA_DER)){
		if(direccion==ARR_DER)
			direccion=ARR_IZQ;
		else
			direccion=ABA_IZQ;
	}else	
	if(ultimaPosX<=minX+radioBola && (direccion==ARR_IZQ||direccion==ABA_IZQ)){
		if(direccion==ARR_IZQ)
			direccion=ARR_DER;
		else
			direccion=ABA_DER;
	}
	if(direccion==ARR_IZQ||direccion==ABA_IZQ){
		ultimaPosX-=10;
	}
	if(direccion==ARR_DER||direccion==ABA_DER){
		ultimaPosX+=10;
	}
	return ultimaPosX;
}

int prox_ball_y(int pos){
	if(maxY==0){
		return ultimaPosY;
	}else	
	if(ultimaPosY>=maxY-radioBola && (direccion==ARR_DER||direccion==ARR_IZQ)){
		if(direccion==ARR_DER)
			direccion=ABA_DER;
		else
			direccion=ABA_IZQ;
	}else	
	if(ultimaPosY<=minY+radioBola && (direccion==ABA_IZQ||direccion==ABA_DER)){
        ultimaPosY=maxY;
        direccion=ARR_IZQ;
        return ultimaPosY;
    }else
    if(choque(pos)==1){
		if(direccion==ABA_DER)
			direccion=ARR_DER;
		else
            direccion=ARR_IZQ;
    }
	if(direccion==ARR_IZQ||direccion==ARR_DER){
		ultimaPosY+=10;
	}
	if(direccion==ABA_IZQ||direccion==ABA_DER){
		ultimaPosY-=10;
	}
	return ultimaPosY;
}
int choque(int pos){
    if(ultimaPosX >= pos-CAR_RADIO && ultimaPosX <= pos+CAR_RADIO){
        if(ultimaPosY <= CAR_Y_DEFAULT + CAR_H){
            return 1;
        }
    }
    return 0;
}
