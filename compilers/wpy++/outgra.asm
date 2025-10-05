section .data
str0 db "Hello GDI", 0
str1 db "white", 0
str2 db "Hello, Python++!", 0

section .text
extern graphics_Init
extern graphics_Clear
extern graphics_DrawText
extern graphics_DrawRect
extern graphics_Loop
global WinMain

WinMain:
    sub rsp, 32
    add rsp, 32
    xor eax, eax
    ret
