#include "draw.h"

void MakeLineBox(TLCD tlcdInfo, Shape shape)
{
    int i, tmp, offset;

    if (shape.start.x > shape.end.x)
    {
        tmp = shape.start.x;
        shape.start.x = shape.end.x;
        shape.end.x = tmp;
    }

    if (shape.start.y > shape.end.y)
    {
        tmp = shape.start.y;
        shape.start.y = shape.end.y;
        shape.end.y = tmp;
    }

    for (i = shape.start.x; i < shape.end.x; i++)
    {
        offset = shape.start.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
        offset = shape.end.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
    }

    for (i = shape.start.y; i < shape.end.y; i++)
    {
        offset = i * 320 + shape.start.x;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
        offset = i * 320 + shape.end.x;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
    }
}