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
    int **position;
} Shape;

typedef void (*drawList)(TLCD, Shape *);

const drawList g_drawTable[] = {
    DrawLine,
    DrawRectangle,
    DrawOval,
    DrawFree};

// make Rectangle Base Code
void DrawRectangle(TLCD tlcdInfo, Shape shape);

void DrawLine(TLCD tlcdInfo, Shape shape);

void DrawOval(TLCD tlcdInfo, Shape shape);

void DrawFree(TLCD tlcdInfo);

#endif