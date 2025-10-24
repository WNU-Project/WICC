section .data
str0 db "Hello, world! (THE FOLLOWING LINE IS A TEST OF ESCAPE SEQUENCES)\n", 0

section .text
extern printf
extern exit
global main

main:
    sub rsp, 40              ; shadow space
    lea rcx, [rel str0]     ; RCX = &string
    call printf
    add rsp, 40
