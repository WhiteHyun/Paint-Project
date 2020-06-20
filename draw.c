#include "draw.h"
#include "ui.h"
#include "list.h"
#include "btn.h"
#include <stdio.h>
#include <stdlib.h>

extern struct List *g_List;
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
    shape->type = TOUCH_LINE;
    int i, offset;
    int startX, startY;
    int endX, endY;
    double incline;    //기울기
    double yIntercept; //y절편
    while (1)          //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure == 0)
        {
            startX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            break;
        }
    }

    tlcdInfo->pressure = -1;

    while (1) //종료지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure == 0)
        {
            endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            break;
        }
    }

    if (startX < endX) //1, 4 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
        yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

        for (i = startX; i <= endX; i++)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
        }
    }

    else //2, 3 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX));
        yIntercept = (double)(endY - incline * endX);

        for (i = startX; i >= endX; i--)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
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
    shape->type = TOUCH_RECT;
    int i, tmp, offset;

    int startX, startY, endX, endY;

    startX = shape->start.x;
    startY = shape->start.y;

    while (1) //종료지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure == 0)
        {
            tlcdInfo->pressure = -1;
            break;
        }
        shape->end.x = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        shape->end.y = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

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
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = shape->end.y * 320 + i;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
        }

        for (i = startY; i < endY; i++)
        {
            offset = i * 320 + startX;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
            offset = i * 320 + endX;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
        }
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
    shape->type = TOUCH_OVAL;
    int i, j, tmp, centerX, centerY, xlen, ylen, offset, x, y;
    int startX, startY, endX, endY;

    startX = shape->start.x;
    startY = shape->start.y;

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
        shape->end.x = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        shape->end.y = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;
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
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }
            }
        }
    }
}

/*
 * This is Base Code for Making DrawFree Made by S.H Hong
 */
void DrawFree(TLCD *tlcdInfo, Shape *shape)
{
    shape->type = TOUCH_FREEDRAW;
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
        for (i = -1; i < 2; i++)
        {
            offset = (ypos + 1) * tlcdInfo->fbvar.xres + xpos + i;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
            shape->position[ypos - START_CANVAS_Y + 1][xpos - START_CANVAS_X + i] = 1;

            offset = ypos * tlcdInfo->fbvar.xres + xpos + i;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
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
    ListClear(); //리스트에 있는 노드들을 전부 제거
    int i, j, offset;

    //캔버스의 존재하는 도형들을 전부 제거
    for (i = START_CANVAS_Y; i < END_CANVAS_Y; i++)
    {
        for (j = START_CANVAS_X; j < END_CANVAS_X; j++)
        {
            offset = i * 320 + j;
            *(tlcdInfo->pfbdata + offset) = WHITE;
        }
    }
    return;
}

/*
 * 구현할 필요 없음. 단순 테이블 설정을 위한 함수
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