GLOBAL cpuVendor
GLOBAL getRTC	
GLOBAL getKey
GLOBAL spkMov
GLOBAL setPIT0Freq
GLOBAL setPIT2Freq
GLOBAL getPIT2Freq
GLOBAL spkStop
GLOBAL haltProcess
GLOBAL fabricateProcessStack
GLOBAL atomicExchange
GLOBAL atomicCompareExchange
GLOBAL atomicAdd
GLOBAL atomicHighValueCheck
GLOBAL atomicLowValueCheck

ALIGN 16

section .text
 ; rdi 1° | rsi 2° | rdx 3° | rcx 4°

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid

	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


; funcion para obtener la hora real de la computadora
getRTC:
	push rbp
	mov rbp, rsp

	push rbx
	xor rax, rax

	; Se deshabilita la interrupción configurando el bit 7 del registro B
    mov al, 0x8B
    out 0x70, al

	; obtengo los segundos
	mov al, 0x00
	out 70h, al
	in al, 71h
	mov [rdi+2], al     ; segundos

	; obtengo los minutos
	mov al, 0x02
	out 70h, al
	in al, 71h
	mov [rdi+1], al     ; minutos

	; obtengo las horas
	mov al, 0x04
	out 70h, al
	in al, 71h
	mov [rdi], al       ; horas

	; Activo las interrupciones seteando el bit 7 del registro B.
	mov al, 0x8A
    out 0x70, al

	pop rbx, 
	mov rsp, rbp
	pop rbp
	mov rax, 3
	ret


getKey: 

    push rbp
    mov rbp, rsp
    mov rax, 0

    ; coloco en la parte baja de eax la tecla que se presiono
    in al, 0x60

    mov rsp, rbp
    pop rbp
    ret

	
setPIT0Freq:
	mov al, 0x36
	out 0x43, al
	mov rax, rdi
	out 0x40, al
	mov al, ah
	out 0x40, al
	ret


setPIT2Freq:
	mov al, 0xB6
	out 0x43, al
	mov rax, rdi
	out 0x42, al
	mov al, ah
	out 0x42, al
	ret


getPIT2Freq:
	mov rax, 0
	push rbx
	mov al, 0b10000000
	out 0x43, al
	in al, 0x42
	mov bl, al
	in al, 0x42
	shl al, 8
	mov al, bl
	pop rbx
	ret


spkMov:
	in al, 0x61
	or al, 0x3
	out 0x61, al
	ret


spkStop:
	in al, 0x61
	and al, 0xFC
	out 0x61, al
	ret

haltProcess:
	hlt
	ret

fabricateProcessStack:
	;receives stackbase, argc, argv and rip as pointers (so rdi, rsi, rdx, rcx respectively)
	;will pass argc as int and argv as pointer (strings have been copied somewhere safe)
	mov r8, rsp		;preserve this stack so i can return later
	and rdi, 0xFFFFFFFFFFFFFFBF ;align to quad-word if not aligned
	mov rsp, rdi	;temp stack switcharoo

	mov rax, 0
	push rax	;20 ss

	push rdi	;19 rsp

	mov rax, 0x202
	push rax	;18 rflags

	mov rax, 0x8
	push rax	;17 cs

	push rcx	;16 RIP -> function to run

	mov rax, 0
	push rax    ;15 rax again
	push rax	;14 rbx
	push rax	;13 rcx
	push rax	;12 rdx

	push rdi	;11 rbp -> stack base
	push rsi	;10 rdi -> argc as 1st argument
	push rdx	;9  rsi -> argv as 2nd argument

	push rax	;8  r8
	push rax	;7  r9
	push rax	;6  r10
	push rax	;5  r11
	push rax	;4  r12
	push rax	;3  r13
	push rax	;2  r14
	push rax	;1  r15
	mov rax, rsp
	mov rsp, r8 ;restore old stack
	ret

atomicExchange:
lock	xchg [rdi], rsi
		ret

atomicCompareExchange:
lock	cmpxchg [rdi], rsi
		ret

atomicAdd:
lock	xadd [rdi], rsi
		ret

atomicHighValueCheck: ; (&value, top, resetValue)
		mov ebx, [rdi]
		;cmp ebx, rsi
		jng end_highcheck
lock	cmpxchg [rdi], rdx
end_highcheck		ret

atomicLowValueCheck: ; (&value, bottom, resetValue)

		mov ebx, [rdi]
		;cmp rsi, ebx
		jng end_lowcheck
lock	cmpxchg [rdi], rdx
end_lowcheck		ret