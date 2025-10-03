section .data
str0 db "Hello, world! (THE FOLLOWING LINE IS A TEST OF ESCAPE SEQUENCES)", 10, 0
str1 db "New Line: ", 10": Tab ", 9" : Backslash \ : Single Quote ' : Double Quote """, 10, 0

section .text
extern printf
extern exit
global main

main:
    sub rsp, 32              ; shadow space
    lea rcx, [rel str0]     ; RCX = &string
    call printf
    add rsp, 32
    sub rsp, 32              ; shadow space
    lea rcx, [rel str1]     ; RCX = &string
    call printf
    add rsp, 32
    mov ecx, 0              ; exit code
    call exit
