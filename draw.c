#include "draw.h"
#include "ui.h"
#include "list.h"

/*
 * This is Base Code for Making Line Made by D.S Kim
 * Make start x , y -> end x , y Line
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Line의 좌표를 특정하여 지워줍니다 (시작할 때 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 )
 */
void DrawLine(TLCD tlcdInfo, Shape *shape)
{
    /* TODO: Draw line */
    printf("DrawLine Executed\n");
}

/*
 * This is Base Code for Making Rectangle Made by T.H Kim
 * Make start x , y -> end x , y Rectange 
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Box를 지워줍니다 (시작할 때 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 ) 
 */
void DrawRectangle(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawRectangle Executed\n");

    int i, tmp, offset;

    int startX, startY, endX, endY;

    startX = shape->start.x;
    startY = shape->start.y;

    endX = shape->end.x;
    endY = shape->end.y;

    if (startX > endX)
    {
        tmp = startX;
        startX = endX;
        endX = tmp;
    }
    if (startY > endY)
    {
        tmp = startY;
        startY = endY;
        endY = tmp;
    }

    for (i = startX; i < endX; i++)
    {
        offset = shape->start.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
        offset = shape->end.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }

    for (i = startY; i < endY; i++)
    {
        offset = i * 320 + startX;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
        offset = i * 320 + endX;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }
}

/*
 * This is Base Code for Making Oval Made by D.E Kim
 * Make start x , y -> end x , y Oval
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Oval을 지워줍니다 (시작할 때 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 )
 */
void DrawOval(TLCD tlcdInfo, Shape *shape)
{
    /* TODO: Draw Oval */
    printf("DrawOval Executed\n");

    int i, j, tmp, centerX, centerY, xlen, ylen, offset, x, y;

    int startX, startY, endX, endY;

    startX = shape->start.x;
    startY = shape->start.y;

    endX = shape->end.x;
    endY = shape->end.y;

    if (startX > endX)
    {
        tmp = startX;
        startX = endX;
        endX = tmp;
    }
    if (startY > endY)
    {
        tmp = startY;
        startY = endY;
        endY = tmp;
    }

    /* set Start and end X , Y */

    centerX = (startX + endX) / 2;
    centerY = (startY + endY) / 2;

    xlen = (startX - centerX) * (startX - centerX); // 선 a의 길이
    ylen = (startY - centerY) * (startY - centerY); // 선 b의 길이

    if (xlen == 0 || ylen == 0)
    {
        printf("error\n");
    }

    // 왜 5 만큼 더하고 빼지않으면 깔끔한 그림이 나오지 않는지 이해가 되지않음 . + 스무스한 모양이아닌 각진모양이나와서 매우마음에 들지않음. 추후 해결예정
    else
    {
        for (i = startY - 5; i < endY + 5; i++)
        {
            for (j = startX - 5; j < endX + 5; j++)
            {
                x = (j - centerX);
                y = (i - centerY);

                if ((x * x) * ylen + (y * y) * xlen <= (xlen * ylen * 1.1) && (x * x) * ylen + (y * y) * xlen >= (xlen * ylen * 0.9))
                {
                    offset = i * 320 + j;
                    *(tlcdInfo.pfbdata + offset) = shape->outColor;
                }
            }
        }
    }
}

/*
 * This is Base Code for Making Oval Made by S.H Hong
 */
void DrawFree(TLCD tlcdInfo, Shape *shape)
{
    /* 아직 미 구현된 상태임 */
    printf("DrawFree Executed\n");

    struct ListNode *node = NULL;
    int pressure, xpos, ypos, x, y;
    while (1)
    {
        if (tlcdInfo.ie.type == 3)
        {
            if (tlcdInfo.ie.code == 0)
            {
                x = tlcdInfo.ie.value;
            }
            else if (tlcdInfo.ie.code == 1)
            {
                y = tlcdInfo.ie.value;
            }
            else if (tlcdInfo.ie.code == 24)
            {
                pressure = tlcdInfo.ie.value;
                if (pressure == 0)
                {
                    xpos = tlcdInfo.a * x + tlcdInfo.b * y + tlcdInfo.c;
                    ypos = tlcdInfo.d * x + tlcdInfo.e * y + tlcdInfo.f;
                }
            }
        }
    }
}

/*
 * TODO
 */
void DrawSelect(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawSelect Executed\n");
    return;
}
/*
 * TODO
 */
void DrawErase(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawErase Executed\n");
    return;
}
/*
 * TODO
 */
void DrawClear(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawClear Executed\n");
    return;
}
/*
 * TODO
 */
void DrawPen(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawPen Executed\n");
    return;
}
/*
 * TODO
 */
void DrawFill(TLCD tlcdInfo, Shape *shape)
{
    printf("DrawFill Executed\n");
    return;
}