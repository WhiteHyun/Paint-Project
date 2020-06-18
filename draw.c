#include "draw.h"
#include "ui.h"

/*
 * This is Base Code for Making Rectangle Made by T.H Kim
 * Make start x , y -> end x , y Rectange 
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Box를 지워줍니다 (시작할떄 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 ) 
 */
void DrawRectangle(TLCD tlcdInfo, Shape shape)
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


/*
 * This is Base Code for Making Rectangle Made by D.S Kim
 * Make start x , y -> end x , y Rectange
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Line의 좌표를 특정하여 지워줍니다 (시작할떄 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 )
 */
void DrawLine(TLCD tlcdInfo, Shape shape)
{
    /* TODO: Draw line */
}

/*
 * This is Base Code for Making Rectangle Made by D.E Kime
 * Make start x , y -> end x , y Rectange
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Oval을 지워줍니다 (시작할떄 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 )
 */
void DrawOval(TLCD tlcdInfo, Shape shape)
{
    /* TODO: Draw Oval */
}