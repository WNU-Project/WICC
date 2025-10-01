section .data
str0 db "Hello GDI",0
str1 db "white",0
str2 db "Hello, Python++!",0

section .text
extern graphics_Init
extern graphics_Clear
extern graphics_DrawText
extern graphics_DrawRect
extern graphics_Loop
global WinMain

WinMain:
    lea rcx, [rel str0]
    mov rdx, 800
    mov r8, 600
    call graphics_Init
    lea rcx, [rel str1]
    call graphics_Clear
    mov rcx, 100
    mov rdx, 120
    lea r8, [rel str2]
    call graphics_DrawText
    mov rcx, 90
    mov rdx, 110
    mov r8, 240
    mov r9, 40
    call graphics_DrawRect
    call graphics_Loop
    xor eax, eax
    ret
