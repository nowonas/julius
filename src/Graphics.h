#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "Data/Types.h"

void Graphics_clearScreen();

void Graphics_drawLine(int x1, int y1, int x2, int y2, Color color);

void Graphics_drawRect(int x, int y, int width, int height, Color color);
void Graphics_drawInsetRect(int x, int y, int width, int height);

void Graphics_fillRect(int x, int y, int width, int height, Color color);

void Graphics_setClipRectangle(int x, int y, int width, int height);

GraphicsClipInfo *Graphics_getClipInfo(int xOffset, int yOffset, int width, int height);

void Graphics_drawImage(int graphicId, int xOffset, int yOffset);

void Graphics_saveScreenshot(const char *filename);

#endif
