#ifndef UI_H
#define UI_H
#include "set.h"
typedef unsigned char ubyte;

enum Color
{
    WHITE = 0xFFFF,
    ORANGE = 0xFE00,
    RED = 0xF800,
    GREEN = 0x968A,
    YELLOW = 0xFFE0,
    NAVY = 0x10C,
    BLUE = 0x398,
    BLACK = 0x0,
    MINT = 0x670
};
enum Size
{
    CANVAS_X = 200,
    CANVAS_Y = 220
};

//기본 UI 출력
void DrawUI(TLCD tlcdInfo);

#endif