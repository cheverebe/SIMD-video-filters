; void sharpness_un_canal_asm (unsigned char *src, unsigned char *dst, int h, int w, int row_size)

global sharpness_un_canal_asm


section .text


%define src ebp + 8
%define dst ebp + 12
%define h ebp + 16
%define w ebp + 20
%define row_size ebp + 24

;variables locales
%define top_row ebp - 4
%define middle_row ebp - 8
%define bottom_row ebp - 12
%define ultimosPixelesDestino ebp -16

%define pixelesPorCiclo 6


sharpness_un_canal_asm:
	push ebp
	mov ebp, esp
	sub esp, 4*4
	push ebx
	push esi
	push edi

	
	mov eax, [src]
	mov [top_row], eax

	add eax, [row_size]
	mov [middle_row], eax

	add eax, [row_size]
	mov [bottom_row], eax

	mov ecx, [h]
	sub ecx, 2

	mov ebx, [row_size]
	sub ebx, [w]	;ebx es la cantidad de basura por fila	

	mov edi, [dst]

	mov eax, [dst]
	add eax, [w]
	add eax, [row_size]
	sub eax, pixelesPorCiclo
	mov [ultimosPixelesDestino], eax

	mov eax, [src]
	add eax, [w]
	sub eax, dword pixelesPorCiclo

  .ciclo:

	call procesarPx
	
	add edi, pixelesPorCiclo
	add dword [top_row], pixelesPorCiclo
	add dword [middle_row], pixelesPorCiclo
	add dword [bottom_row], pixelesPorCiclo

	cmp [top_row], eax
	jb .ciclo

	;termine una fila
	mov [top_row], eax
	dec dword [top_row]

	mov edx, [row_size]

	mov [middle_row], eax
	add [middle_row], edx
	dec dword [middle_row]

	mov [bottom_row], eax
	add [bottom_row], edx
	add [bottom_row], edx
	dec dword [bottom_row]

	;modifico edi
	mov edi, [ultimosPixelesDestino]

	call procesarPx

	;vuelvo los punteros a la columna 0
	inc dword [top_row]
	inc dword [middle_row]
	inc dword [bottom_row]

	mov edx, [row_size]
	add [ultimosPixelesDestino], edx

	add edi, pixelesPorCiclo
	add dword [top_row], pixelesPorCiclo
	add dword [middle_row], pixelesPorCiclo
	add dword [bottom_row], pixelesPorCiclo

	;sumo basuras
	add edi, ebx
	add dword [top_row], ebx
	add dword [middle_row], ebx
	add dword [bottom_row], ebx

	add eax, [row_size]

	dec ecx
	jnz .ciclo
	
salir_ciclo:

  .salir:
	pop edi
	pop esi
	pop ebx
	add esp, 4*4
	pop ebp
	ret


  procesarPx:
	pxor xmm7, xmm7
	;armo la primer colmna
	mov esi, [top_row]
	movq xmm0, [esi]
	punpcklbw xmm0, xmm7 

	mov esi, [middle_row]
	movq xmm1, [esi]
	punpcklbw xmm1, xmm7
	movups xmm4, xmm1			;guardo los originales para usarlos despues
	psllw xmm1, 1		;*2

	mov esi, [bottom_row]
	movq xmm2, [esi]
	punpcklbw xmm2, xmm7
	
	paddw xmm0, xmm1
	paddw xmm0, xmm2		;primer columna sumada

	;armo la segunda columna
	mov esi, [top_row]
	inc esi
	movq xmm1, [esi]
	punpcklbw xmm1, xmm7
	psllw xmm1, 1		;*2

	mov esi, [middle_row]
	inc esi
	movq xmm2, [esi]
	punpcklbw xmm2, xmm7
	psllw xmm2, 2		;*4

	mov esi, [bottom_row]
	inc esi
	movq xmm3, [esi]
	punpcklbw xmm3, xmm7
	psllw xmm3, 1		;*2
	
	paddw xmm1, xmm2
	paddw xmm1, xmm3


	paddw xmm0, xmm1		;col1 + col2

	;armo la tercer columna
	mov esi, [top_row]
	add esi, 2
	movq xmm1, [esi]
	punpcklbw xmm1, xmm7

	mov esi, [middle_row]
	add esi, 2
	movq xmm2, [esi]
	punpcklbw xmm2, xmm7
	psllw xmm2, 1		;*2

	mov esi, [bottom_row]
	add esi, 2
	movq xmm3, [esi]
	punpcklbw xmm3, xmm7
	
	paddw xmm1, xmm2
	paddw xmm1, xmm3


	paddw xmm0, xmm1	;listo, calculados los pixeles
	psrlw xmm0, 4		;divido por 16
	
	;los originales 2*(xmm4) - xmm0
	psllw xmm4, 1
	psubusw xmm4, xmm0	;substract unsigned saturation
	movups xmm0, xmm4

	packuswb xmm0, xmm7	;los bytes mas bajos ya estan listos

	movq [edi], xmm0

	ret
