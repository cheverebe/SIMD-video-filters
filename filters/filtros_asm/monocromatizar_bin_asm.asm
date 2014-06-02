# void monocromatizar_bin_asm(unsigned char *src, unsigned char *dst, int h, int w, int src_row_size, int umbral);

global monocromatizar_bin_asm

%define src ebp + 8
%define dst ebp + 12
%define h ebp + 16
%define w ebp + 20
%define row_size ebp + 24
%define umbral ebp + 28


section .data


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

monocromatizar_bin_asm:
	
	iniciar_func
        
        mov ecx, 8
        
        movzx eax, byte [umbral]
        pxor xmm7, xmm7
        movd xmm7, eax  ; xmm7 = 00 | 00 | ... | umbral

        movups xmm0, xmm7
poner_umbral:
        pslldq xmm0, 2
        paddb xmm0, xmm7
        loop poner_umbral
        movups xmm7, xmm0       ; xmm7 = umbral | ... | umbral

        xor edx, edx    ; edx <- contador fila
        xor ecx, ecx    ; ecx <- contador columna
        mov eax, [src]  ; eax <- puntero al inicio de fila de src
        mov ebx, [src]  ; ebx <- puntero al pixel de src
        mov esi, [dst]  ; esi <- puntero al inicio de fila de dst
        mov edi, [dst]  ; edi <- puntero al pixel de dst
	    pxor xmm5, xmm5
    		
ciclo_norm:
        movq xmm0, [ebx]                      	; xmm0 <- 8 bytes a procesar

        punpcklbw xmm0, xmm5					; convierto a words
        movups xmm1, xmm0
        psubusw xmm1, xmm7                      ; Resto contra el umbral para que solo queden los mayores a el
        pcmpgtw xmm1, xmm5                      ; xmm1 = A los mayores al umbral los dejo en ff
        packsswb xmm1, xmm5						; convierto de vuelta a bytes
        
        movq [edi], xmm1
        
        add ebx, 8
        add edi, 8
        add ecx, 8
        
        cmp ecx, [w]
        jb ciclo_norm

        add eax, [row_size]
        mov ebx, eax
        
        add esi, [row_size]
        mov edi, esi
        
        xor ecx, ecx
        inc edx
        cmp edx, [h]
        jb ciclo_norm
        
        terminar_func
        ret
	
