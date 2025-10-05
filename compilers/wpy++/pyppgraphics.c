// pyppgraphics.c
#include <windows.h>
#include <string.h>
#include <strings.h>
#include "pyppgraphics.h"

// Global state (simple demo; you can encapsulate later)
static const char *g_title = "Python++";
static int g_width = 640, g_height = 480;

static const char *g_text = NULL;
static int g_textX = 0, g_textY = 0;

static RECT g_rect = {0};
static int g_rectActive = 0;

static COLORREF g_bg = RGB(0xFF, 0xFF, 0xFF);
static HWND g_hwnd = NULL;

static COLORREF map_color(const char *name) {
    if (!name) return RGB(0,0,0);
    if (strcmp(name, "black") == 0)  return RGB(0,0,0);
    if (strcmp(name, "white") == 0)  return RGB(255,255,255);
    if (strcmp(name, "red") == 0)    return RGB(220,20,60);
    if (strcmp(name, "green") == 0)  return RGB(34,139,34);
    if (strcmp(name, "blue") == 0)   return RGB(30,144,255);
    return RGB(0,0,0);
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Background
        HBRUSH bg = CreateSolidBrush(g_bg);
        FillRect(hdc, &ps.rcPaint, bg);
        DeleteObject(bg);

        // Rectangle
        if (g_rectActive) {
            HBRUSH h = CreateSolidBrush(RGB(230, 230, 230));
            HBRUSH old = SelectObject(hdc, h);
            Rectangle(hdc, g_rect.left, g_rect.top, g_rect.right, g_rect.bottom);
            SelectObject(hdc, old);
            DeleteObject(h);
        }

        // Text
        if (g_text) {
            SetBkMode(hdc, TRANSPARENT);
            TextOutA(hdc, g_textX, g_textY, g_text, (int)lstrlenA(g_text));
        }

        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void graphics_Init(const char *title, int width, int height) {
    if (title) g_title = title;
    g_width = width;
    g_height = height;

    WNDCLASSA wc = {0};
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = GetModuleHandleA(NULL);
    wc.lpszClassName = "PyppGraphicsWindow";
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClassA(&wc);

    g_hwnd = CreateWindowExA(
        0, wc.lpszClassName, g_title,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, g_width, g_height,
        NULL, NULL, wc.hInstance, NULL
    );

    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
}

void graphics_Clear(const char *color) {
    g_bg = map_color(color);
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, TRUE);
}

void graphics_DrawText(int x, int y, const char *text) {
    g_text = text;
    g_textX = x;
    g_textY = y;
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, FALSE);
}

void graphics_DrawRect(int x, int y, int w, int h) {
    g_rect.left = x;
    g_rect.top  = y;
    g_rect.right  = x + w;
    g_rect.bottom = y + h;
    g_rectActive = 1;
    if (g_hwnd) InvalidateRect(g_hwnd, NULL, FALSE);
}

void graphics_Loop(void) {
    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
