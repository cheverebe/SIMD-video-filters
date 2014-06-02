; void aplicarContraste_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size, int dst_row_size, int nivel)

global aplicarContraste_asm

section .data
	val dd 0
	cien dd 100

	cerocomacinco dd 0.5, 0.5, 0.5, 0.5
	cerocomacinconeg dd -0.5, -0.5, -0.5, -0.5
	doscincocinco dd 255.0, 255.0, 255.0, 255.0

section .text


%define src ebp + 8
%define dst ebp + 12
%define h ebp + 16
%define w ebp + 20
%define row_size ebp + 24
%define dsp_row_size ebp + 28
%define nivel ebp + 32

%define BYTES_POR_ITER 4

aplicarContraste_asm:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi


	finit
	fild dword [nivel]
	fiadd dword [cien]
	fidiv dword [cien]
	fmul st0, st0
	fst dword [val]
	;val = ((nivel +100) /2)^100
	
	movups xmm6, [val]
	shufps xmm6, xmm6, 0
	;xmm6 = [val, val, val, val] en floats
	

	mov ebx, [row_size]
	sub ebx, [w]
	sub ebx, [w]
	sub ebx, [w]	;ebx es la cantidad de basura por fila	

	mov esi, [src]				;uso esi para recorrer la matriz fuente
	mov edi, [dst]				;uso edi para recorrer la matriz destino
	mov eax, [row_size]


	mov ecx, [h]						;ecx es mi contador de filas

	;creo las mascaras

	;xmm3 = 0.5 0.5 0.5 0.5	en float
	movups xmm3, [cerocomacinco]
	
	;xmm4 = -0.5 -0.5 -0.5 -0.5
	movups xmm4, [cerocomacinconeg]
	
	;xmm5 = 255 255 255 255 en float
	movups xmm5, [doscincocinco]
	
	;xmm7 = 0 enteros
	pxor xmm7, xmm7

  ciclo:
  

	ciclo_fila:

		cmp eax, BYTES_POR_ITER
		jb limite
		
		movq xmm0, [esi]
		punpcklbw xmm0, xmm7		;xmm7 = 0
		punpcklwd xmm0, xmm7
		cvtdq2ps xmm0, xmm0			;tengo R1,B0,G0,R0 en flotante
		

		divps xmm0, xmm5		; /255
		addps xmm0, xmm4		; -0.5
		mulps xmm0, xmm6
		addps xmm0, xmm3		; +0,5
		mulps xmm0, xmm5		; *255

		;convierto de vuelta a char unsigned

		cvtps2dq xmm0, xmm0
		packssdw xmm0, xmm0
		packuswb xmm0, xmm0


		movd [edi], xmm0		;muevo los 4 bytes mas bajos

		sub eax, BYTES_POR_ITER
		add esi, BYTES_POR_ITER
		add edi, BYTES_POR_ITER
		jmp ciclo_fila

	limite:
		cmp dword eax, 0
		je fin_fila
		
		;empaquetar con signed saturation
		;movd [edi], xmm0
		;add edi, BYTES_POR_ITER
		;sub eax, BYTES_POR_ITER

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
