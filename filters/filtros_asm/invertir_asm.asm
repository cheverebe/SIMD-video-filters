; void invertir_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size)

global invertir_asm


%define src [ebp+8]
%define dst [ebp+12]
%define h [ebp+16]
%define w [ebp+20]
%define row [ebp+24]

;variables locales
%define src_col [ebp-4]
%define dst_col [ebp-8]
%define check_pad [ebp-12]

section .data

sat dq 0xffffffffffffffff, 0xffffffffffffffff

section .text

invertir_asm:
;armo la pila
	push ebp
	mov ebp,esp
	sub esp, 12
	push esi
	push edi
	push ebx

;bajo los parametros
	mov esi, src				;uso esi para recorrer la matriz fuente
	mov edi, dst				;uso edi para recorrer la matriz destino
	mov eax, row

	mov dword src_col, esi			;en una variable local guardo el puntero a columna de src
	mov dword dst_col, edi			;en una variable local guardo el puntero a columna de dst

	mov ecx, h				;ecx es mi contador de filas

ciclo:
	mov dword check_pad, eax		;guardo en una variable local el row que le vo a ir restand ed a 16 para controlar el caso borde

	ciclo_fila:

		cmp dword check_pad, 16
		jb limite
     	movups xmm0, [sat]			;xmm0 = 255|255|255|255|255|255|255|255
		movups xmm1,[esi]			;xmm1 = p15|p14|p13|p12|p11|p10|p9|p8|p7|p6|p5|p4|p3|p2|p1|p0
		psubusb xmm0, xmm1			;xmm0 = 255-p15|......|255-p0

		sub dword check_pad, 16
		movups [edi], xmm0
		add esi, 16
		add edi, 16
		jmp ciclo_fila

		limite:
		cmp dword check_pad, 0
		je fin_fila
		movd [edi], xmm0
		add edi, 4
		psrldq xmm0, 4
		sub dword check_pad, 4
		jmp limite

	fin_fila:

	;avanzo la fila y miro si termine

		add dword src_col, eax
		mov esi, src_col
		add dword dst_col, eax
		mov edi, dst_col
		loop ciclo

;desarmo la pila
	pop ebx
	pop edi
	pop esi
	add esp, 12
	pop ebp
	ret
