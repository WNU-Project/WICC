// pyppgraphics.h
#ifndef PYPPGRAPHICS_H
#define PYPPGRAPHICS_H

#ifdef __cplusplus
extern "C" {
#endif

void graphics_Init(const char *title, int width, int height);
void graphics_Clear(const char *color);
void graphics_DrawText(int x, int y, const char *text);
void graphics_DrawRect(int x, int y, int w, int h);
void graphics_Loop(void);

#ifdef __cplusplus
}
#endif

#endif
