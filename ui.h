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
    CYAN = 0x670
};

enum Size
{
    SIZEOF_CANVAS_X = 200,
    SIZEOF_CANVAS_Y = 220,
};

enum Position
{
    START_CANVAS_X = 60,
    END_CANVAS_X = 260,
    START_CANVAS_Y = 10,
    END_CANVAS_Y = 230
};

//기본 UI 출력
void DrawUI(TLCD tlcdInfo);

#endif
