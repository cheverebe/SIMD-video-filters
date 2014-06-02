;void reemplazarFondo(unsigned char* src_one, unsigned char* src_two, unsigned char* back, unsigned char* dst, int height, int width, int src_row_size, int tolerance);

global reemplazarFondo

%define src_one ebp + 8
%define src_two ebp + 12
%define back ebp + 16
%define dst ebp + 20
%define h ebp + 24
%define w ebp + 28
%define src_row_size ebp + 32
%define tolerance ebp + 36


section .data

	src1_fila dd 0
	src1_pixel dd 0
	
	src2_fila dd 0
	src2_pixel dd 0
	
	src_fondo_pixel dd 0
	src_fondo_fila dd 0
	
section .text

%macro iniciar_func 0
        push ebp
        mov ebp, esp
        sub esp, 4
        push esi
        push edi
        push ebx
%endmacro

%macro terminar_func 0
        pop ebx
        pop edi
        pop esi
        add esp, 4
        pop ebp
%endmacro

reemplazarFondo:
	
	iniciar_func
        
    xor ecx, ecx
    mov ecx, 16

	mov eax, [w]
	add [w], eax
	add [w], eax
        
    movzx eax, byte [tolerance]
    pxor xmm7, xmm7
    movd xmm7, eax  ; xmm7 = 00 | 00 | ... | umbral

    movups xmm0, xmm7
poner_umbral:
    pslldq xmm0, 2
    paddb xmm0, xmm7
    loop poner_umbral
    movups xmm7, xmm0       ; xmm7 = tolerancia | ... | tolerancia
    
    movups xmm6, xmm7
    pxor xmm5, xmm5
    psubw xmm5, xmm6
    movups xmm6, xmm5		;xmm6 = -tolerancia | ... | -tolerancia


    xor edx, edx    ; edx <- contador fila
    xor ecx, ecx    ; ecx <- contador columna
    
    mov eax, [src_one]
    mov dword [src1_fila], eax  ;puntero al inicio de fila de src1
	mov dword [src1_pixel], eax	;puntero al pixel de src1
        
    mov ebx, [src_two]
    mov dword [src2_fila], ebx	;puntero al inicio de fila de src2
    mov dword [src2_pixel], ebx ;puntero al pixel de src2
    
    mov eax, [back]
    mov dword [src_fondo_fila], eax
    mov dword [src_fondo_pixel], eax
    
    mov esi, [dst]  ; esi <- puntero al inicio de fila de dst
    mov edi, [dst]  ; edi <- puntero al pixel de dst
		
ciclo_norm:
	mov ebx, dword [src1_pixel]
    movq xmm0, [ebx]                
    pxor xmm5, xmm5
    punpcklbw xmm0, xmm5
    movups xmm1, xmm0						;xmm1 = pixeles de src1
    
    mov eax, dword [src2_pixel]
    movq xmm0, [eax]                
    pxor xmm5, xmm5
    punpcklbw xmm0, xmm5
    movups xmm2, xmm0						;xmm2 = pixeles de src2
    
    psubsw xmm1, xmm2						;src1 - src2
    
    ;aplico la tolerancia por arriba
    
    movups xmm2, xmm1
    
    psubusw xmm1, xmm7                      ; Resto contra el umbral para que solo queden los mayores a el
    pcmpgtw xmm1, xmm5                      ; xmm1 = A los mayores al umbral los dejo en ff
    
    ;aplico la tolerancia por abajo
	paddsw xmm2, xmm7
    pcmpgtw xmm2, xmm5
    pcmpeqw xmm2, xmm5	;esto se hace para invertir el resultado
			; en realidad se quiere ver si xmm2 < xmm5
    
    ;junto los 2 resultados
    por xmm1, xmm2
	;paso a bytes
	packsswb xmm1, xmm5
	
	; hacer un and entre los 3 canales. Todos tienen que ser parecidos
	movups xmm2, xmm1
	movups xmm3, xmm1
	;shift xmm2 left 1byte
	psrldq xmm2, 1
	pand xmm1, xmm2
	
	;shift left xmm3, 2bytes
	psrldq xmm3, 2
	pand xmm1, xmm3
	
;		obtener los 2 pixeles P
	;xmm1 = .....P . . P
	movups xmm2, xmm1
	psrldq xmm2, 2
	por xmm1, xmm2
	
	
	;extiendo los 2 bytes a 6 (BGR BGR)
	movups xmm2, xmm1
	pslldq xmm2, 2
	por xmm1, xmm2
	pslldq xmm2, 2
	por xmm1, xmm2

	;Tengo los bytes en 0x00 que tienen que ser reemplazados por el fondo
	movups xmm3, xmm1

	mov eax, [src_fondo_pixel]
	movups xmm2, [eax]
	pandn xmm1, xmm2
	
	;los que estan en 0xFF los reemplazo por la imagen src1
	mov eax, [src1_pixel]
	movups xmm2, [eax]
	pand xmm3, xmm2
	
	;junto
	por xmm1, xmm3

	movups [edi], xmm1
        
        
    add dword [src1_pixel], 6
    add dword [src2_pixel], 6
    add dword [src_fondo_pixel], 6
	add edi, 6
    add ecx, 6
    
    cmp ecx, [w]
    jb ciclo_norm

	;actualizo posicion de src1
	mov eax, [src1_fila]
	add eax, [src_row_size]
	mov [src1_fila], eax
	mov [src1_pixel], eax

	;actualizo la posicion de src2
	mov eax, [src2_fila]
	add eax, [src_row_size]
	mov [src2_fila], eax
	mov [src2_pixel], eax
	
	;actualizo el iterador de la imagen de fondo
	mov eax, [src_fondo_fila]
	add eax, [src_row_size]
	mov [src_fondo_fila], eax
	mov [src_fondo_pixel], eax
        
        
	;actualizo la posicion de dst
    add esi, [src_row_size]
    mov edi, esi
    
    xor ecx, ecx
    inc edx
    cmp edx, [h]
    jb ciclo_norm
    
    terminar_func
    ret
	
