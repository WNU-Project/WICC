section .data
fmt_str db "%s", 0
str0 db "Hello, world! (THE FOLLOWING LINE IS A TEST OF ESCAPE SEQUENCES)", 10, 0
str1 db "New Line: ", 10": Tab ", 9" : Backslash \ : Single Quote ' : Double Quote """, 10, 0

section .text
extern printf
extern exit
global main

main:
    sub rsp, 32              ; shadow space
    lea rcx, [rel fmt_str]   ; RCX = "%s"
    lea rdx, [rel str0]     ; RDX = &string
    call printf
    add rsp, 32
    sub rsp, 32              ; shadow space
    lea rcx, [rel fmt_str]   ; RCX = "%s"
    lea rdx, [rel str1]     ; RDX = &string
    call printf
    add rsp, 32
    sub rsp, 32              ; shadow space
    mov ecx, 0              ; RCX = exit code
    call exit
    add rsp, 32
