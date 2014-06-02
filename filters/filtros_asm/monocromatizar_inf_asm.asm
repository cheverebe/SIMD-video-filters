; void monocromatizar_inf_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int dst_row_size)

global monocromatizar_inf_asm


%define src ebp + 8
%define dst ebp + 12
%define h ebp + 16
%define w ebp + 20
%define src_row_size ebp + 24
%define dst_row_size ebp + 28

%define ultimoPxSrc ebp - 4
%define ultimoPxDst ebp - 8

%define pixelesPorCiclo 5


section .data

	mascaraShuffle db 00000000b, 00000011b, 00000110b, 00001001b, 00001100b, \
						0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff


section .text

monocromatizar_inf_asm:
	push ebp
	mov ebp, esp
	sub esp, 8
	push ebx
	push esi
	push edi	
	
	mov esi, [src]
	mov edi, [dst]

	mov eax, [src]
	sub eax, pixelesPorCiclo * 3
	add eax, [w]
	add eax, [w]
	add eax, [w]		;w * 3
	mov [ultimoPxSrc], eax

	mov eax, [dst]
	sub eax, pixelesPorCiclo
	add eax, [w]
	mov [ultimoPxDst], eax

	mov eax, [w]
	mov ebx, 3
	mul ebx

	mov ebx, [src_row_size]
	sub ebx, eax			;ebx es la cantidad de basura en el source

	mov edx, [dst_row_size]
	sub edx, dword [w]		;edx = cantidad de basura en dest

	mov ecx, [h]	;contador de filas

	

  .ciclo:
	call procesarPx

	add esi, pixelesPorCiclo * 3
	add edi, pixelesPorCiclo
	
	cmp esi, [ultimoPxSrc]
	jb .ciclo

	mov esi, [ultimoPxSrc]
	mov edi, [ultimoPxDst]
	call procesarPx

	mov eax, [src_row_size]
	add [ultimoPxSrc], eax

	mov eax, [dst_row_size]
	add [ultimoPxDst], eax

	add esi, pixelesPorCiclo * 3
	add esi, ebx
	add edi, pixelesPorCiclo
	add edi, edx

	loop .ciclo



  .salir:
	pop edi
	pop esi
	pop ebx
	add esp, 8
	pop ebp
	ret	


  procesarPx:
	movups xmm0, [esi]
	movdqu xmm1, xmm0
	movdqu xmm2, xmm0
	psrldq xmm1, 1
	psrldq xmm2, 2
	
	pmaxub xmm0, xmm1
	pmaxub xmm0, xmm2
	
	;xmm0 = x,x,x,P x,x,P,x x,P,x,x P,x,x,P

	movups xmm1, [mascaraShuffle]
	pshufb xmm0, xmm1

	movq [edi], xmm0

	ret	
