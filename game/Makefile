CC = gcc
ASM = nasm
DBG = gdb
CFLAGS = -ggdb -Wall -std=c99 -pedantic -m32 
ASMFLAGS = -felf32 -g -F dwarf
OPENCV_LIB = opencv
GTK_LIB = gtk+-2.0
LIBS = `pkg-config --cflags --libs $(OPENCV_LIB) $(GTK_LIB)`

OBJS_C = filtros_c/buscarPos_c_code.o filtros_c/hacerAuto_c_code.o filtros_c/hacerPelota_c_code.o
	
OBJS_ASM = filtros_asm/diff_asm.o filtros_asm/filtro_reemplazo_fondo_asm.o filtros_asm/contarblancos_asm.o ../filtros/filtros_asm/suavizar_asm.o ../filtros/filtros_asm/unir_canales_asm.o ../filtros/filtros_asm/separar_canales_asm.o
	
%.o: %.asm
	$(ASM) $(ASMFLAGS) -o $@ $<
	
%.o: %.c
	$(CC) $(CFLAGS) $(LIBS) -c -o $@ $<

all: $(OBJS_ASM) $(OBJS_C)
	$(CC) $(CFLAGS) main.c $(OBJS_ASM) $(OBJS_C) -o main $(LIBS)

clean:
	rm -f main
	rm -f *.o
	rm -f filtros_c/*.o
