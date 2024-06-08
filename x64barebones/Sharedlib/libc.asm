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
GLOBAL malloc
GLOBAL free
GLOBAL SyscallMemInfo
GLOBAL SyscallPrintMem
GLOBAL execv
GLOBAL exit
GLOBAL SyscallProcessInfo
GLOBAL SyscallNice
GLOBAL SyscallToggleBlockProcess
GLOBAL waitpid
GLOBAL yield
GLOBAL getPID
GLOBAL kill
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

malloc:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0B
    mov rbx, rdi
    
    int 80h

    mov rsp, rbp
    pop rbp
    ret

free:
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

execv:
    push rbp
    mov rbp, rsp    

    mov rax, 0x0F
    mov rbx, rdi    ; rip
    mov rcx, rsi    ; argv
                    ; rdx (mode) in place
    int 80h

    mov rsp, rbp
    pop rbp
    ret

SyscallProcessInfo:
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

SyscallToggleBlockProcess:
    push rbp
    mov rbp, rsp    

    mov rax, 0x12
    mov rbx, rdi    ; pid
    int 80h

    mov rsp, rbp
    pop rbp
    ret
exit:
    push rbp
    mov rbp, rsp    

    mov rax, 0x13
    mov rbx, rdi    ; value
    int 80h

    mov rsp, rbp
    pop rbp
    ret

waitpid:
    push rbp
    mov rbp, rsp    

    mov rax, 0x14
    mov rbx, rdi    ; pid
    mov rcx, rsi    ; wstatus
    int 80h

    mov rsp, rbp
    pop rbp
    ret
yield:
    push rbp
    mov rbp, rsp    

    mov rax, 0x15
    int 80h

    mov rsp, rbp
    pop rbp
    ret
getPID:
    push rbp
    mov rbp, rsp    

    mov rax, 0x16
    int 80h

    mov rsp, rbp
    pop rbp
    ret

kill:
    push rbp
    mov rbp, rsp    

    mov rax, 0x17
    mov rbx, rdi    ; pid
    int 80h

    mov rsp, rbp
    pop rbp
    ret
invalidOpcode:
    ud2
    ret
