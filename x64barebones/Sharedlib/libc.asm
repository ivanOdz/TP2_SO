GLOBAL SyscallWrite
GLOBAL SyscallRead
GLOBAL SyscallClear
GLOBAL SyscallGetRegisters
GLOBAL SyscallGetRTC
GLOBAL invalidOpcode
GLOBAL SyscallGetFormat
GLOBAL SyscallSetFormat
GLOBAL SyscallPutBlock
GLOBAL SyscallGetTicks
GLOBAL SyscallAudio
GLOBAL SyscallSetTimer
GLOBAL SyscallMalloc
GLOBAL SyscallFree
GLOBAL SyscallMemInfo
GLOBAL SyscallPrintMem
GLOBAL SyscallCreateProcess
GLOBAL SyscallPs
GLOBAL SyscallNice
ALIGN 16


section .text

SyscallGetRTC:
    push rbp
    mov rbp, rsp    

    mov rax, 0
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallClear:
    push rbp
    mov rbp, rsp    

    mov rax, 1
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallGetRegisters:
    push rbp
    mov rbp, rsp
    push rax    

    mov rax, 2
    int 80h

    pop rax
    mov rsp, rbp
    pop rbp
    ret

SyscallRead:
    push rbp
    mov rbp, rsp    

    mov rax, 3
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret
SyscallWrite:
    push rbp
    mov rbp, rsp    

    mov rax, 4
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallGetFormat:
    push rbp
    mov rbp, rsp    

    mov rax, 5
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallSetFormat:
    push rbp
    mov rbp, rsp    

    mov rax, 6
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallPutBlock:
    push rbp
    mov rbp, rsp    

    mov rax, 7
    mov rbx, rdi    ;struct*
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallGetTicks:
    push rbp
    mov rbp, rsp    

    mov rax, 8
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallAudio:
    push rbp
    mov rbp, rsp    

    mov rax, 9
    mov rbx, rdi
    mov rcx, rsi
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallSetTimer:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0A
    mov rbx, rdi    ;struct*
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallMalloc:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0B
    mov rbx, rdi
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallFree:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0C
    mov rbx, rdi
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret
SyscallMemInfo:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0D
    mov rbx, rdi
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallPrintMem:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0E
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallCreateProcess:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0F
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallPs:
    push rbp
    mov rbp, rsp    

    mov rax, 0x10
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallNice:
    push rbp
    mov rbp, rsp    

    mov rax, 0x11
    mov rbx, rdi    ; pid
    mov rcx, rsi    ; newPriority
    int 80h

    mov rsp, rbp
    pop rbp
    ret

invalidOpcode:
    ud2
    ret
