#ifndef UI_H
#define UI_H
#include "set.h"
typedef unsigned char ubyte;

unsigned short MakePixel(ubyte r, ubyte g, ubyte b);
//기본 UI 출력
void DrawUI(TLCD tlcdInfo);

#endif