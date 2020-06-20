#include "draw.h"
#include "ui.h"
#include "list.h"
#include "btn.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * This is Base Code for Making Line Made by D.S Kim
 * Make start x , y -> end x , y Line
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Line의 좌표를 특정하여 지워줍니다 (시작할 때 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 )
 */
void DrawLine(TLCD *tlcdInfo, Shape *shape)
{
    int i, tmp, offset;
    int startXpos, startYpos;
    int endXpos, endYpos;
    double incline;    //기울기
    double yIntercept; //y절편

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure == 0)
        {
            startXpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startYpos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            break;
        }
    }

    tlcdInfo->pressure = -1;

    while (1) //종료지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure == 0)
        {
            endXpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endYpos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            break;
        }
    }

    if (startXpos < endXpos) //1, 4 사분면
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos)); //기울기 = y증가량 / x증가량
        yIntercept = (double)(endYpos - incline * endXpos);                                //y절편 = y - 기울기 * x

        for (i = startXpos; i <= endXpos; i++)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo->pfbdata + offset) = 0;
        }
    }

    else //2, 3 사분면
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos));
        yIntercept = (double)(endYpos - incline * endXpos);

        for (i = startXpos; i >= endXpos; i--)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo->pfbdata + offset) = 0;
        }
    }
}

/*
 * This is Base Code for Making Rectangle Made by T.H Kim
 * Make start x , y -> end x , y Rectange
 * 필요한기능  -> 입력받은 점이 처음 찍힌 점일경우 start x, y에 적립
 *             -> 아닐경우 end의 x,y좌표를 계속하여 갱신하여줍니다.
 *             -> 갱신했을시 이전에 그려진 Box를 지워줍니다 (시작할 때 집어주면 될듯?)
 *             -> ( 이 기능이 핵심적임 ) 
 */
void DrawRectangle(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawRectangle Executed\n");

    int i, tmp, offset;

    if (shape->start.x > shape->end.x)
    {
        tmp = shape->start.x;
        shape->start.x = shape->end.x;
        shape->end.x = tmp;
    }

    if (shape->start.y > shape->end.y)
    {
        tmp = shape->start.y;
        shape->start.y = shape->end.y;
        shape->end.y = tmp;
    }

    for (i = shape->start.x; i < shape->end.x; i++)
    {
        offset = shape->start.y * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        offset = shape->end.y * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;
    }

    for (i = shape->start.y; i < shape->end.y; i++)
    {
        offset = i * 320 + shape->start.x;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        offset = i * 320 + shape->end.x;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;
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
void DrawOval(TLCD *tlcdInfo, Shape *shape)
{
    /* TODO: Draw Oval */
    printf("DrawOval Executed\n");
}

/*
 * This is Base Code for Making DrawFree Made by S.H Hong
 */
void DrawFree(TLCD *tlcdInfo, Shape *shape)
{
    /* 아직 미 구현된 상태임 */
    printf("DrawFree Executed\n");

    struct ListNode *node = NULL;
    int xpos, ypos, i, offset;
    //도형 크기 동적 할당
    shape->position = (int **)malloc(sizeof(int *) * SIZEOF_CANVAS_Y); //캔버스의 y크기: 220
    for (i = 0; i < 220; i++)
    {
        shape->position[i] = (int *)malloc(sizeof(int) * SIZEOF_CANVAS_X); //캔버스의 x크기: 200
    }

    while (1)
    {
        /* 터치 입력을 받음 */
        InputTouch(tlcdInfo);
        if (tlcdInfo->pressure == 0)
        {
            tlcdInfo->pressure = -1;
            break;
        }
        /*코드 구현*/
        xpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        ypos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;
        // printf("xpos: %d\nypos: %d\n", xpos, ypos);
        //*(tlcdInfo->pfbdata + offset) = BLACK;
        for (i = -3; i < 3; i++)
        {
            offset = (ypos + i) * tlcdInfo->fbvar.xres + xpos;
            *(tlcdInfo->pfbdata + offset) = BLACK;
            shape->position[ypos - START_CANVAS_Y + i][xpos - START_CANVAS_X] = 1;

            offset = ypos * tlcdInfo->fbvar.xres + xpos + i;
            *(tlcdInfo->pfbdata + offset) = BLACK;
            shape->position[ypos - START_CANVAS_Y][xpos - START_CANVAS_X + i] = 1;
        }
    }
}

/*
 * TODO
 */
void DrawSelect(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawSelect Executed\n");

    return;
}

/*
 * TODO
 */
void DrawErase(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawErase Executed\n");

    return;
}

/*
 * TODO
 */
void DrawClear(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawClear Executed\n");

    return;
}

/*
 * TODO
 */
void DrawPen(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawPen Executed\n");

    return;
}

/*
 * TODO
 */
void DrawFill(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawFill Executed\n");

    return;
}