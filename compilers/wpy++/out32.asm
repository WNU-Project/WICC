section .data
str0 db Hello, world! (THE FOLLOWING LINE IS A TEST OF ESCAPE SEQUENCES)
, 0
str1 db New Line: 
: Tab 	 : Backslash \ : Single Quote ' : Double Quote "
, 0

section .text
extern printf
extern exit
global main

main:
    push str0
    call printf
    add esp, 4
    push str1
    call printf
    add esp, 4
    push 0
    call exit
