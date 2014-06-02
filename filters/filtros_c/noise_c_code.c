void noise_c (unsigned char* src_one, unsigned char* src_two, unsigned char* noise,int height, int width, int row_size ){
		
    int noise_red = 0;
    int noise_blue = 0;
    int noise_green = 0;

	for (int j = 0; j < height; j++){
		for (int i = 0; i < 3*width; i+=3){
			
            noise_blue  =  (src_one[    i+row_size*j] + src_two[   i+row_size*j])/2;
            noise_green =  (src_one[1 + i+row_size*j] + src_two[1+ i+row_size*j])/2;
            noise_red   =  (src_one[2 + i+row_size*j] + src_two[2+ i+row_size*j])/2;

            noise[i + row_size * j    ] = noise_blue;
            noise[i + row_size * j + 1] = noise_green;
            noise[i + row_size * j + 2] = noise_red;

		}
	}
}
