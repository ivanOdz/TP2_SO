GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL int80_handler

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception06Handler
GLOBAL syscall_getRegisters

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN printRegs
EXTERN getStackBase
EXTERN syscall_playSound
EXTERN syscall_getTicks
EXTERN syscall_putBlock
EXTERN syscall_getFormat
EXTERN syscall_setFormat
EXTERN syscall_write
EXTERN syscall_read
EXTERN syscall_clear
EXTERN syscall_getRTC
EXTERN syscall_setTimer
EXTERN syscall_malloc
EXTERN syscall_free
EXTERN syscall_meminfo
EXTERN syscall_printMemory

SECTION .text

%macro pushState 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	pushState
	push rax
	mov rax, [rsp + 8]		;r15
	mov [regs], rax
	mov rax, [rsp + 2*8]	;r14
	mov [regs + 8], rax
	mov rax, [rsp + 3*8]	;r13
	mov [regs + 2*8], rax
	mov rax, [rsp + 4*8]	;r12
	mov [regs + 3*8], rax
	mov rax, [rsp + 5*8]	;r11
	mov [regs + 4*8], rax
	mov rax, [rsp + 6*8]	;r10
	mov [regs + 5*8], rax
	mov rax, [rsp + 7*8]	;r9
	mov [regs + 6*8], rax
	mov rax, [rsp + 8*8]	;r8
	mov [regs + 7*8], rax
	mov rax, [rsp + 9*8]	;rsi
	mov [regs + 8*8], rax
	mov rax, [rsp + 10*8]	;rdi
	mov [regs + 9*8], rax
	mov rax, [rsp + 22*8]	;rbp (original, pusheado antes del rax)
	mov [regs + 10*8], rax
	mov rax, [rsp + 12*8]	;rdx
	mov [regs + 11*8], rax
	mov rax, [rsp + 13*8]	;rcx
	mov [regs + 12*8], rax
	mov rax, [rsp + 14*8]	;rbx
	mov [regs + 13*8], rax
	mov rax, [rsp + 21*8]	;rax
	mov [regs + 14*8], rax	
	mov rax, [rsp + 23*8]	;rip
	mov [regs + 15*8], rax
	mov rax, [rsp + 17*8]	;cs
	mov [regs + 16*8], rax
	mov rax, [rsp + 18*8]	;rflags
	mov [regs + 17*8], rax
	mov rax, [rsp + 11*8]	;rsp (original, que esta en rbp del frame)
	mov [regs + 18*8], rax
	mov rax, [rsp + 20*8]	;ss
	mov [regs + 19*8], rax
	pop rax

	mov rdi, %1  ; pasaje de parametro
	mov r12, rax ; se mueven a estos registros porque deben ser preservados por funciones
	mov r13, rcx ; (me garantizo que no me los van a tocar)
	mov r15, rdx ; y yo estoy preservandolos con pushState y popState
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	push rax
	mov al, 20h
	out 20h, al
	pop rax
	popState
%endmacro



%macro exceptionHandler 1
	push rax
	pushState
	mov rdi, %1 		; pasaje de parametro
	call exceptionDispatcher

	popState
	pop rax
    call getStackBase	
	mov [rsp+24], rax
    mov rax, userland
	mov [rsp], rax		; PISO la dirección de retorno
	
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


; 8254 Timer (Timer Tick)
_irq00Handler:
	push rax
	irqHandlerMaster 0
	pop rax
	iretq

; Keyboard
_irq01Handler:
	push rax
	irqHandlerMaster 1
	pop rax
	iretq

; Cascade pic never called
_irq02Handler:
	push rax
	irqHandlerMaster 2
	pop rax
	iretq

; Serial Port 2 and 4
_irq03Handler:
	push rax
	irqHandlerMaster 3
	pop rax
	iretq

; Serial Port 1 and 3
_irq04Handler:
	push rax
	irqHandlerMaster 4
	pop rax
	iretq

; USB
_irq05Handler:
	push rax
	irqHandlerMaster 5
	pop rax
	iretq

; INT 80h
_irq80Handler:
	push rax	; syscall id
    irqHandlerMaster 6
	pop rdx
	iretq


; Zero Division Exception
_exception0Handler:
	exceptionHandler 0

; Invalid Opcode Exception
_exception06Handler:
    exceptionHandler 1

getSyscall:
	mov rax, [syscalls + 8*rdi]
	ret
int80_handler:
    push rbp               ; armado del stack frame
    mov rbp, rsp
    mov rsi, syscalls
	mov rax, [rsi + 8*r12]

    mov rdi, rbx           ; cargo los argumentos de la syscall
    mov rsi, r13
	mov rdx, r15
    call rax               ; llamo a la syscall.
    mov rsp, rbp           ; desarmo el stack
    pop rbp
    ret

syscall_getRegisters:
	push QWORD[regs + 8*20]
	push QWORD[regs + 8*19]
	push QWORD[regs + 8*18]
	push QWORD[regs + 8*17]
	push QWORD[regs + 8*16]
	push QWORD[regs + 8*15]
	push QWORD[regs + 8*14]
	push QWORD[regs + 8*13]
	push QWORD[regs + 8*12]
	push QWORD[regs + 8*11]
	push QWORD[regs + 8*10]
	push QWORD[regs + 8*9]
	push QWORD[regs + 8*8]
	push QWORD[regs + 8*7]
	push QWORD[regs + 8*6]
	push QWORD[regs + 8*5]
	push QWORD[regs + 8*4]
	push QWORD[regs + 8*3]
	push QWORD[regs + 8*2]
	push QWORD[regs + 8*1]
	push QWORD[regs + 8*0]
	mov rdi, 1
	call printRegs
	add rsp, 21*8
	mov rax, 0
	ret

haltcpu:
	cli
	hlt
	ret

SECTION .text
	syscalls dq syscall_getRTC, syscall_clear, syscall_getRegisters, syscall_read, syscall_write, syscall_getFormat, syscall_setFormat, syscall_putBlock, syscall_getTicks, syscall_playSound, syscall_setTimer, syscall_malloc, syscall_free, syscall_meminfo, syscall_printMemory

SECTION .bss
	aux resq 1
    regs resq 21


SECTION .rodata
	userland equ 0x400000
