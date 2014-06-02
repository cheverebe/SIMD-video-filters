#ifndef SUAVIZAR_C_C
#define SUAVIZAR_C_C

void suavizar_c (unsigned char *src, unsigned char *dst, int h, int w, int row_size) {

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
				unsigned short result = *(src - row_size - 1);
				
				result += *(src - row_size) << 1;
				result += *(src - row_size + 1);
				result += *(src - 1) << 1;
				result += *(src) << 2;
				result += *(src + 1) << 1;
				result += *(src + row_size - 1);
				result += *(src + row_size) << 1;
				result += *(src + row_size + 1);
				
				*dst = (unsigned char)(result >> 4);
			}
			src++;
			dst++;
		}
		src += row_size - w;
		dst += row_size - w;
	}
}

#endif
