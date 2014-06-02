; void noise_asm( unsigned char* src, unsigned char* src2, unsigned char* dst, int h, int w, int row_size);

global noise_asm

section .data
	val dd 0
	cien dd 100
        dos dd 2.0, 2.0, 2.0, 2.0

section .text


%define src ebp + 8
%define src2 ebp + 12
%define dst ebp + 16
%define h ebp + 20
%define w ebp + 24
%define row_size ebp + 28

;variables locales
%define row ebp - 4

%define BYTES_POR_ITER 8

noise_asm:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi

	mov ebx, [row_size]
	sub ebx, [w]
	sub ebx, [w]
	sub ebx, [w]	;ebx es la cantidad de basura por fila	

        mov esi, [src]				;uso esi para recorrer la 1ra matriz fuente
        mov edx, [src2]				;uso edx para recorrer la otra matriz fuente
        mov [row], edx				;uso edx para recorrer la otra matriz fuente
        mov edi, [dst]				;uso edi para recorrer la matriz destino
	mov eax, [row_size]


	mov ecx, [h]						;ecx es mi contador de filas

        ;creo las mascaras

	;xmm5 = 2 2 2 2 en float
	movups xmm5, [dos]
	
	;xmm7 = 0 enteros
	pxor xmm7, xmm7

  ciclo:

	ciclo_fila:

		cmp eax, BYTES_POR_ITER
                jb fin_fila
		
                movups xmm0, [esi]
		punpcklbw xmm0, xmm7		;xmm7 = 0

                mov edx, [row]				;uso edx para recorrer la otra matriz fuente

                movups xmm1, [edx]

                punpcklbw xmm1, xmm7		;xmm7 = 0

                paddw xmm0, xmm1		; sumo ambos pixels

                psrlw xmm0, 1
		
		;convierto de vuelta a char unsigned

                packuswb xmm0, xmm0


                movups [edi], xmm0		;muevo los 4 bytes mas bajos

		sub eax, BYTES_POR_ITER
		add esi, BYTES_POR_ITER

                mov edx, [row]				;uso edx para recorrer la otra matriz fuente
                add edx, BYTES_POR_ITER
                mov [row], edx				;uso edx para recorrer la otra matriz fuente

                add edi, BYTES_POR_ITER
		jmp ciclo_fila

	fin_fila:

	;avanzo la fila y miro si termine

		add esi, ebx
		add edi, ebx
		mov eax, [row_size]
		loop ciclo

;desarmo la pila
	pop ebx
	pop edi
	pop esi
	pop ebp
	ret

