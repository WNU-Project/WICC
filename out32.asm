section .data
str0 db "Hello, world! (THE FOLLOWING LINE IS A TEST OF ESCAPE SEQUENCES)\n", 0

section .text
extern printf
extern exit
global main

main:
    push str0
    call printf
    add esp, 4
