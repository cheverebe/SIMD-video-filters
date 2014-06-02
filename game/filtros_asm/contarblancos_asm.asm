; int contarblancos_asm (unsigned char *src, int h, int w, int row_size)
; Devuelve la cantidad de pixeles que estan en 0xFF, la imagen fuente tiene que ser binaria
; o sea solamente contener pixeles 0x00 y 0xFF

global contarblancos_asm


%define src [ebp+8]
%define h [ebp+12]
%define w [ebp+16]
%define row [ebp+20]

;variables locales
%define src_col [ebp-4]
%define check_pad [ebp-12]

section .data

sat dq 0xfefefefefefefefe, 0xfefefefefefefefe

section .text

contarblancos_asm:
;armo la pila
	push ebp
	mov ebp,esp
	sub esp, 12
	push esi
	push edi
	push ebx

;bajo los parametros
	mov esi, src				;uso esi para recorrer la matriz fuente
	mov eax, row

	mov dword src_col, esi		;en una variable local guardo el puntero a columna de src
	mov ecx, h					;ecx es mi contador de filas
	
	pxor xmm7, xmm7
	xor edx, edx

ciclo:
	mov dword check_pad, eax		;guardo en una variable local el row que le voy a ir 
									;restando de a 16 para controlar el caso borde

	ciclo_fila:
		cmp dword check_pad, 16
		jb limite
		
     	movups xmm0, [sat]			;xmm0 = 254|254|254|254|254|254|254|254
		movups xmm1,[esi]			;xmm1 = p15|p14|p13|p12|p11|p10|p9|p8|p7|p6|p5|p4|p3|p2|p1|p0
		psubusb xmm1, xmm0			;xmm0 = p15-254|......|p0-254
		
		movups xmm2, xmm1
		punpcklbw xmm1, xmm7
		
		psrldq xmm2, 8
		punpcklbw xmm2, xmm7
		
		;suma horizontal y acumulo el resultado en edx		
		phaddw xmm1, xmm2
		phaddw xmm1, xmm1
		phaddw xmm1, xmm1
		phaddw xmm1, xmm1
		
		punpcklwd xmm1, xmm7
		
		movd ebx, xmm1
		add edx, ebx

		sub dword check_pad, 16
		add esi, 16
		jmp ciclo_fila

		limite:
		cmp dword check_pad, 0
		je fin_fila
		psrldq xmm0, 4
		sub dword check_pad, 4
		jmp limite

	fin_fila:

	;avanzo la fila y miro si termine

		add dword src_col, eax
		mov esi, src_col
		loop ciclo

;desarmo la pila
	pop ebx
	pop edi
	pop esi
	add esp, 12
	pop ebp
	
	;devuelvo el acumulado de pixeles en 0xFF
	mov eax, edx	
	
	ret
