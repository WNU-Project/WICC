section .data
str0 db "Hello, world!", 0

section .text
extern printf
extern exit
global main

main:
    push str0
    call printf
    add esp, 4
    push 0
    call exit
