; void modificar_brillo_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size, int dst_row_size, int offset)

global modificar_brillo_asm

section .data
	efes dq 0xffffffffffffffff, 0xffffffffffffffff
	mascara db 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1

section .text


%define src ebp + 8
%define dst ebp + 12
%define h ebp + 16
%define w ebp + 20
%define row_size ebp + 24
%define dsp_row_size ebp + 28
%define offset ebp + 32


modificar_brillo_asm:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi


	mov ebx, [row_size]
	sub ebx, [w]
	sub ebx, [w]
	sub ebx, [w]	;ebx es la cantidad de basura por fila	

	mov esi, [src]				;uso esi para recorrer la matriz fuente
	mov edi, [dst]				;uso edi para recorrer la matriz destino
	mov eax, [row_size]


	mov ecx, [h]						;ecx es mi contador de filas

	;creo las mascaras
	pxor xmm7, xmm7
	
	xor edx, edx
	mov edx, 0xffffffff
	movd xmm6, edx
	punpcklbw xmm6, xmm7
	movups xmm5, [mascara]
	pshufb xmm6, xmm5
	
	xor edx, edx
	mov edx, [offset]
	shl edx, 16
	mov dx, word [offset]
	movd xmm4, edx
	pshufd xmm1, xmm4, 0

  ciclo:
  

	ciclo_fila:

		cmp eax, 6
		jb limite
		
		;mover pixeles a xmm0
		movq xmm0, [esi]
		punpcklbw xmm0, xmm7		;xmm7 = 0
		
		;mover y repetir offset a xmm1 en words
		
		
		paddsw xmm0, xmm1
		
		;saturar con min/max 0, 255
		pminsw xmm0, xmm6; [mascara255w]
		pmaxsw xmm0, xmm7; [mascara0w]

		pxor xmm2, xmm2
		movups xmm3, xmm0
		packuswb xmm3, xmm2
		movlpd [edi], xmm3		;muevo los 8 bytes mas bajos

		sub eax, 6
		add esi, 6
		add edi, 6
		jmp ciclo_fila

	limite:
		cmp dword eax, 0
		je fin_fila
		
		;empaquetar con signed saturation
		;movd [edi], xmm0
		;add edi, 4
		;sub eax, 4

		jmp fin_fila

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
