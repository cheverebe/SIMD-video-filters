void hacerPelota_c (unsigned char* car, unsigned char* out,int pos,int desplazamiento,int height, int width, int src_row_size,int car_row_size ){
    int ci=0;
    int cj=0;
int hCar=20;
int wCar=20;

for (int j = height-hCar-desplazamiento; j < height-desplazamiento; j++){
    for (int i = pos-(wCar/2); i < pos+(wCar/2); i++){

        cj= j - ( height - hCar - desplazamiento);
        ci= i - ( pos    - (wCar/2));

        out[i*3 +       src_row_size * j] = car[    3*ci + car_row_size*cj];
        out[1   + i*3 + src_row_size * j] = car[1 + 3*ci + car_row_size*cj];
        out[2   + i*3 + src_row_size * j] = car[2 + 3*ci + car_row_size*cj];
    }
}
}
/*

void hacerPelota_c (unsigned char* ball, unsigned char* out,int posX,int posY, int src_row_size,int radio,int ball_row_size){
	int ci;
	int cj;

	for (int j = posY-radio; j < posY+radio; j++){
	    for (int i = posX-radio; i < posX+radio; i++){

		cj= j - ( posY - radio);
		ci= i - ( posX - radio);

		out[i*3 +       src_row_size * j] = ball[    3*ci + ball_row_size*cj];
		out[1   + i*3 + src_row_size * j] = ball[1 + 3*ci + ball_row_size*cj];
		out[2   + i*3 + src_row_size * j] = ball[2 + 3*ci + ball_row_size*cj];
	    }
	}
}
*/
