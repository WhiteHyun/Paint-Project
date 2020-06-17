#ifndef DRAW_H
#define DRAW_H

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
    int shape;
    Point start;
    Point end;
    unsigned short inColor;  // inBound Color
    unsigned short outColor; // outBound Color
    // int matrix
} Shape;

// make Rectangle Base Code
void MakeLineBox(TLCD tlcdInfo, Shape shape);
#endif