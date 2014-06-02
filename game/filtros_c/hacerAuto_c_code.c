void hacerAuto_c (unsigned char* car, unsigned char* out,int pos,int height, int width, int src_row_size,int car_row_size ){
    int ci=0;
    int cj=0;
int hCar=18;
int wCar=90;
int desplazamiento=50;

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
