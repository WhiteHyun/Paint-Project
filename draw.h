#ifndef DRAW_H
#define DRAW_H

#include "set.h"

typedef struct _Point
{
    int x;
    int y;
} Point;

/*
 * Shape infomation struct.
 * shape -> 0 is Pen , 1 is Line , 2 Rectangle , 3 Oval , 4 FreeDraw
 * Point start & end have each x , y pos
 * inColor is inBound Color
 * outColor is outBound Color
 */
typedef struct _Shape
{
    int type;
    Point start;
    Point end;
    unsigned short inColor;  // inBound Color
    unsigned short outColor; // outBound Color
    int **position;          //FreeDraw array

    int moveX; //parallel translation variable
    int moveY; //parallel translation variable
} Shape;

typedef void (*drawList)(TLCD *, Shape *);

struct Pixel sketchBook[SIZEOF_CANVAS_Y][SIZEOF_CANVAS_X];

void DrawLine(TLCD *tlcdInfo, Shape *shape);

// make Rectangle Base Code
void DrawRectangle(TLCD *tlcdInfo, Shape *shape);

void DrawOval(TLCD *tlcdInfo, Shape *shape);

void DrawFree(TLCD *tlcdInfo, Shape *shape);

void DrawSelect(TLCD *tlcdInfo, Shape *shape);

void DrawErase(TLCD *tlcdInfo, Shape *shape);

void DrawClear(TLCD *tlcdInfo, Shape *shape);

void DrawPen(TLCD *tlcdInfo, Shape *shape);

void DrawFill(TLCD *tlcdInfo, Shape *shape);

/*
 * 도형과 관련된 함수들의 테이블입니다.
 */
static const drawList g_drawTable[] =
    {
        DrawLine,
        DrawRectangle,
        DrawOval,
        DrawFree,
        DrawSelect,
        DrawErase,
        DrawClear,
        DrawPen,
        DrawFill};

#endif
