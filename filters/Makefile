CC = gcc
ASM = nasm
DBG = gdb
CFLAGS = -ggdb -Wall -std=c99 -pedantic -m32 
ASMFLAGS = -felf32 -g -F dwarf
OPENCV_LIB = opencv
GTK_LIB = gtk+-2.0
LIBS = `pkg-config --cflags --libs $(OPENCV_LIB) $(GTK_LIB)` -lrt


OBJS_ASM = filtros_asm/invertir_asm.o filtros_asm/monocromatizar_inf_asm.o filtros_asm/separar_canales_asm.o filtros_asm/suavizar_asm.o filtros_asm/brillo_asm.o filtros_asm/contraste_asm.o filtros_asm/sharpness_un_canal_asm.o filtros_asm/unir_canales_asm.o filtros_asm/noise_asm.o filtros_asm/monocromatizar_bin_asm.o filtros_asm/borde_lap_asm.o


OBJS_C = filtros_c/unir_canales_c.o  filtros_c/borde_lap_c.o filtros_c/separar_canales_c.o filtros_c/brillo_c.o filtros_c/contraste_c.o filtros_c/noise_c_code.o filtros_c/invertir_c.o



%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
	
%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

all: $(OBJS_ASM) $(OBJS_C)
	$(CC) $(CFLAGS) main.c $(OBJS_ASM) $(OBJS_C) -o main $(LIBS)


clean:
	rm -f main
	rm -f *.o
	rm -f filtros_asm/*.o
	rm -f filtros_c/*.o
