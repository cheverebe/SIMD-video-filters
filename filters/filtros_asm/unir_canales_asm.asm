; void unir_canales_asm (unsigned char *dst, unsigned char *src_r, unsigned char *src_g, unsigned char *src_b, int m, int n, int dst_row_size, int src_row_size)

global unir_canales_asm

%define ptr_dst ebp+8
%define ptr_src_r ebp+12
%define ptr_src_g ebp+16
%define ptr_src_b ebp+20
%define h ebp+24
%define w ebp+28
%define src_row_size ebp+32
%define dst_row_size ebp+36

%define base_fila ebp-4
%define base_fila_src_r ebp-8
%define base_fila_src_g ebp-12
%define base_fila_src_b ebp-16
%define padding ebp-20
%define padding_dst ebp-24

section .data

sat dq 0xffffffffffffffff, 0xffffffffffffffff
maskb dq 0xFF02FFFF01FFFF00, 0xFFFFFF04FFFF03FF
maskg dq 0x02FFFF01FFFF00FF, 0xFFFF04FFFF03FFFF
maskr dq 0xFFFF01FFFF00FFFF, 0xFF04FFFF03FFFF02

masklb dq 0x0000000002000001
masklg dq 0x0000000200000100
masklr dq 0x0000020000010000

section .text

unir_canales_asm:

    ;------------stackframe
    push ebp
    mov ebp, esp
    sub esp, 12
    push esi
    push edi
    push ebx
    ;------------levanto parametros
    mov ecx, [h]                ; h(altura) lo tomo como contador de filas
    mov eax, [dst_row_size]     ; eax <- anchoFilaColor
    mov ebx, [src_row_size]     ; ebx <- anchoFilaBN

    mov edi, [ptr_dst]      ; edi <- imgColor
    mov [base_fila], edi    ;

    mov esi, [ptr_src_r]        ;esi <-
    mov [base_fila_src_r], esi
    mov esi, [ptr_src_g]
    mov [base_fila_src_g], esi
    mov esi, [ptr_src_b]
    mov [base_fila_src_b], esi

    ;calculo el padding
    mov edx, [w]	;tengo en edx w=tamaño fila monocromatica sin padding
mov [padding], ebx  ;guardo en el padding el tamaño de la fila
    sub [padding], edx  ;le resto el tamaño sin padding y me queda el padding
	

   mov edx, [w]        ;tengo en edx w=tamaño fila monocromatica sin padding
   add edx, [w]
   add edx, [w]        ;tengo en edx w*3=tamaño fila son padding
mov [padding_dst], eax  ;guardo en el padding el tamaño de la fila
    sub [padding_dst], edx  ;le resto el tamaño sin padding y me queda el padding

    ;----------checkeo si es vacia
    cmp ecx, 0
    je fin
mov eax,ebx

ciclo_fila:

    ciclo_col:
        cmp eax, 16
        jb limite

        ;cargo 3 veces la tira de 16 bytes
	
	mov esi, [ptr_src_r]
        movups xmm0, [esi]      ;R
        
	mov esi, [ptr_src_g]
        movups xmm1, [esi]       ;G
        
	mov esi, [ptr_src_b]
        movups xmm2, [esi]       ;B

	;cargo las mascaras de shuffle
        movups xmm3, [maskr]
        movups xmm4, [maskg]
        movups xmm5, [maskb]

        ;shuffle
        pshufb xmm0, xmm3
        pshufb xmm1, xmm4
        pshufb xmm2, xmm5
        ;aca tengo en los primeros 5 bytes de cada color en el xmm respecivo
        mov edi, [ptr_dst]
	paddw xmm0, xmm1
	paddw xmm0, xmm2
        movups [edi], xmm0

        ;marco que cargue 5 pixeles
        sub eax, 5
        ;sub eax, 2
        ;avanzo los punteros
        add dword [ptr_dst], 15
        add dword [ptr_src_r], 5
        add dword [ptr_src_g], 5
        add dword [ptr_src_b], 5

    jmp ciclo_col

    limite:
        ;----bajo una fila los punteros base
        ;----actualizo esi y edi
        mov ebx, [src_row_size]
        add [base_fila_src_r], ebx
        add [base_fila_src_g], ebx
        add [base_fila_src_b], ebx

	;pongo en ebx el tamaño de fila destino para mover la base fila destino
	mov ebx, [dst_row_size]
        add [base_fila], ebx
        mov edi, [ptr_dst]
        ;recupero el valor de src row size
        mov ebx, [src_row_size]
ciclo_lim:

        ;grabo b
        mov esi, [ptr_src_b]
        mov dl, [esi]
        mov [edi], dl
        add edi, 1
        mov edx,1
        add [ptr_src_b], edx

        ;grabo g
        mov esi, [ptr_src_g]
        mov dl, [esi]
        mov [edi], dl
        add edi, 1
        mov edx,1
        add [ptr_src_g], edx

        ;grabo r
        mov esi, [ptr_src_r]
        mov dl, [esi]
        mov [edi], dl
        add edi, 1
        mov edx,1
        add [ptr_src_r], edx

        sub eax, 1

        cmp eax,0
    jg ciclo_lim

        ;----pongo a eax nuevamente como src_row_size para un nuevo ciclo
        mov eax, [src_row_size]
	
        ;recupero el valor de src row size(EAX)
	mov ebx, eax
        mov edx, [base_fila_src_r]
        mov [ptr_src_r], edx

        mov edx, [base_fila_src_g]
        mov [ptr_src_g], edx

        mov edx, [base_fila_src_b]
        mov [ptr_src_b], edx

        mov edx, [base_fila]
        mov [ptr_dst], edx

    fin_fila:

;loop ciclo_fila
dec ecx
		cmp ecx, 0
        jg ciclo_fila

fin:
    ;------------stackframe
    pop ebx
    pop edi
    pop esi
    add esp, 12
    pop ebp
	ret
