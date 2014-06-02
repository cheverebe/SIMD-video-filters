; void separar_canales_asm (unsigned char *src, unsigned char *dst_r, unsigned char *dst_g, unsigned char *dst_b, int m, int n, int src_row_size, int dst_row_size)

global separar_canales_asm

%define ptr_src ebp+8
%define ptr_dst_r ebp+12
%define ptr_dst_g ebp+16
%define ptr_dst_b ebp+20
%define h ebp+24
%define w ebp+28
%define src_row_size ebp+32
%define dst_row_size ebp+36

%define base_fila ebp-4
%define base_fila_dst_r ebp-8
%define base_fila_dst_g ebp-12
%define base_fila_dst_b ebp-16
%define padding ebp-20
%define padding_dst ebp-24

section .data

sat dq 0xffffffffffffffff, 0xffffffffffffffff
maskb dq 0x0000000c09060300
maskg dq 0x0000000d0a070401
maskr dq 0x0000000e0b080502

masklb dq 0x0000000d0a070401
masklg dq 0x0000000e0b080502
masklr dq 0x0000000f0c090603

section .text

separar_canales_asm:

    ;------------stackframe
    push ebp
    mov ebp, esp
    sub esp, 12
    push esi
    push edi
    push ebx
    ;------------levanto parametros
    mov esi, [ptr_src]      ;--a esi lo uso para recorrer
    mov edi, [ptr_dst_r]
    mov ecx, [h]
    mov eax, [dst_row_size]
    mov ebx, [src_row_size]
    mov [base_fila], esi
    ;mov [base_fila_dst], edi
    mov edi, [ptr_dst_r]
    mov [base_fila_dst_r], edi
    mov edi, [ptr_dst_g]
    mov [base_fila_dst_g], edi
    mov edi, [ptr_dst_b]
    mov [base_fila_dst_b], edi

    ;calculo el padding
    mov edx, [w]
    add edx, [w]
    add edx, [w]        ;tengo en edx w*3=tamaño fila son padding
    mov [padding], ebx  ;guardo en el padding el tamaño de la fila
    sub [padding], edx  ;le resto el tamaño sin padding y me queda el padding

    mov edx, [w]        ;tengo en edx w*3=tamaño fila son padding
    mov [padding_dst], eax  ;guardo en el padding el tamaño de la fila
    sub [padding_dst], edx  ;le resto el tamaño sin padding y me queda el padding

    ;----------checkeo si es vacia
    cmp ecx, 0
    je fin

ciclo_fila:

    ciclo_col:
        cmp ebx, 16
        jb limite

        ;cargo 3 veces la tira de 16 bytes
        movups xmm0, [esi]      ;R
        movups xmm1, xmm0       ;G
        movups xmm2, xmm0       ;B
        ;cargo las mascaras de shuffle
        movq xmm3, [maskr]
        movq xmm4, [maskg]
        movq xmm5, [maskb]
        ;shuffle
        pshufb xmm0, xmm3
        pshufb xmm1, xmm4
        pshufb xmm2, xmm5
        ;aca tengo en los primeros 5 byes de cada color en el xmm respecivo
        mov edi, [ptr_dst_r]
        movq [edi], xmm0
        mov edi, [ptr_dst_g]
        movq [edi], xmm1
        mov edi, [ptr_dst_b]
        movq [edi], xmm2

        sub ebx, 15
        add esi, 15
        add dword [ptr_dst_r], 5
        add dword [ptr_dst_g], 5
        add dword [ptr_dst_b], 5

    jmp ciclo_col

    limite:
        ;----bajo una fila los punteros base
        ;----actualizo esi y edi
        mov ebx, [dst_row_size]
        add [base_fila_dst_r], ebx
        add [base_fila_dst_g], ebx
        add [base_fila_dst_b], ebx
        ;mov edi, [base_fila_dst]

        mov edi, [base_fila_dst_r]
        mov [ptr_dst_r], edi
        mov edi, [base_fila_dst_g]
        mov [ptr_dst_g], edi
        mov edi, [base_fila_dst_b]
        mov [ptr_dst_b], edi
        ;----pongo a ebx nuevamente como src_row_size para un nuevo ciclo
        mov ebx, [src_row_size]
        add [base_fila], ebx
        mov esi, [base_fila]

        ;pongo esi = fin-16 y edi = fin_dst - 16
        sub esi, [padding]
        sub esi, 16
        sub dword [ptr_dst_r], 5
        sub dword [ptr_dst_g], 5
        sub dword [ptr_dst_b], 5
        mov edx, [padding_dst]
        sub [ptr_dst_r], edx
        sub [ptr_dst_g], edx
        sub [ptr_dst_b], edx

        ;cargo 3 veces la tira de 16 bytes
        movups xmm0, [esi]      ;R
        movups xmm1, xmm0       ;G
        movups xmm2, xmm0       ;B
        ;cargo las mascaras de shuffle
        movq xmm3, [masklr]
        movq xmm4, [masklg]
        movq xmm5, [masklb]
        ;shuffle
        pshufb xmm0, xmm3
        pshufb xmm1, xmm4
        pshufb xmm2, xmm5
        ;aca tengo en los primeros 5 byes de cada color en el xmm respecivo

        ;grabo r
        mov edi, [ptr_dst_r]
        movd edx, xmm0
        mov [edi], edx
        psrldq xmm0, 4               ;(SHIFTEO)
        movd edx, xmm0
        mov [edi+4], dl

        ;grabo G
        mov edi, [ptr_dst_g]
        movd edx, xmm1
        mov [edi], edx
        psrldq xmm1, 4               ;(SHIFTEO)
        movd edx, xmm1
        mov [edi+4], dl
        ;grabo B
        mov edi, [ptr_dst_b]
        movd edx, xmm2
        mov [edi], edx
        psrldq xmm2, 4               ;(SHIFTEO)
        movd edx, xmm2
        mov [edi+4], dl

        ;grabo
        ;pongo esi = fin-16 y edi = fin_dst - 16
        mov edx, [padding_dst]
        add [ptr_dst_r], edx
        add [ptr_dst_g], edx
        add [ptr_dst_b], edx
        add dword [ptr_dst_r], 5
        add dword [ptr_dst_g], 5
        add dword [ptr_dst_b], 5

        add esi, [padding]
        add esi, 16

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
