	static int ultimaPos=0;

int buscarPos_c (unsigned char* src_one, unsigned char* src_two,int height, int width, int src_row_size ){
	
	
	int diff_red = 0;
	int diff_blue = 0;
	int diff_green = 0;
	
	int tolerance = 30;
	int j = height/2;
	int start=0;
	int end=0;
		for (int i = 0; i < 3*width; i+=3){
			
			diff_blue = (src_one[i+src_row_size*j] - src_two[i+src_row_size*j]);
			diff_green = (src_one[1+i+src_row_size*j] - src_two[1+i+src_row_size*j]);
			diff_red = (src_one[2+i+src_row_size*j] - src_two[2+i+src_row_size*j]);

			if ((diff_blue >tolerance) || (diff_red > tolerance) || (diff_green > tolerance)
                || (diff_blue < -tolerance) || (diff_red < -tolerance) || (diff_green < -tolerance)){
                if(start==0){
                    start=i/3;
                }
            }else{
                if(start!=0&&end==0){
                    end=i/3;
                }
		if(end-start<15){
			start=0;
			end=0;
		}
            }
       }
if(start==0&&end==0){
	return ultimaPos;
}
if(start!=0 && end==0)
	end=width;
   ultimaPos= (start+end)/2;
   return (start+end)/2;
}
