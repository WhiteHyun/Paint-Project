#include "draw.h"
#include "ui.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

inline void InputTouch(TLCD *tlcdInfo)
{
    read(tlcdInfo->fd, &tlcdInfo->ie, sizeof(struct input_event));

    if (tlcdInfo->ie.type == 3)
    {
        if (tlcdInfo->ie.code == 0)
        {
            tlcdInfo->x = tlcdInfo->ie.value;
        }

        else if (tlcdInfo->ie.code == 1)
        {
            tlcdInfo->y = tlcdInfo->ie.value;
        }

        else if (tlcdInfo->ie.code == 24)
        {
            tlcdInfo->pressure = tlcdInfo->ie.value;
        }
    }
}

/*
 * This is Base Code for Making Line Made by D.S Kim
 * Make start x , y -> end x , y Line
 * �ʿ��ѱ��  -> �Է¹��� ���� ó�� ���� ���ϰ�� start x, y�� ����
 *             -> �ƴҰ�� end�� x,y��ǥ�� ����Ͽ� �����Ͽ��ݴϴ�.
 *             -> ���������� ������ �׷��� Line�� ��ǥ�� Ư���Ͽ� �����ݴϴ� (������ �� �����ָ� �ɵ�?)
 *             -> ( �� ����� �ٽ����� )
 */
void DrawLine(TLCD tlcdInfo, Shape *shape)
{
    int i, tmp, offset;
    int startXpos, startYpos;
    int endXpos, endYpos;
    double incline;    //����
    double yIntercept; //y����

    while (1) //���������� x, y��ǥ �Է�
    {
        InputTouch(&tlcdInfo);

        if (tlcdInfo.pressure == 0)
        {
            startXpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
            startYpos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;

            break;
        }
    }

    tlcdInfo.pressure = -1;

    while (1) //���������� x, y��ǥ �Է�
    {
        InputTouch(&tlcdInfo);

        if (tlcdInfo.pressure == 0)
        {
            endXpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
            endYpos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;

            break;
        }
    }

    if (startXpos < endXpos) //1, 4 ��и�
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos)); //���� = y������ / x������
        yIntercept = (double)(endYpos - incline * endXpos);                                //y���� = y - ���� * x

        for (i = startXpos; i <= endXpos; i++)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo.pfbdata + offset) = 0;
        }
    }

    else //2, 3 ��и�
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos));
        yIntercept = (double)(endYpos - incline * endXpos);

        for (i = startXpos; i >= endXpos; i--)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo.pfbdata + offset) = 0;
        }
    }
}

/*
 * This is Base Code for Making Rectangle Made by T.H Kim
 * Make start x , y -> end x , y Rectange
 * �ʿ��ѱ��  -> �Է¹��� ���� ó�� ���� ���ϰ�� start x, y�� ����
 *             -> �ƴҰ�� end�� x,y��ǥ�� ����Ͽ� �����Ͽ��ݴϴ�.
 *             -> ���������� ������ �׷��� Box�� �����ݴϴ� (������ �� �����ָ� �ɵ�?)
 *             -> ( �� ����� �ٽ����� )
 */
void DrawRectangle(TLCD tlcdInfo, Shape *shape)
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
        *(tlcdInfo.pfbdata + offset) = shape->outColor;

        offset = shape->end.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }

    for (i = shape->start.y; i < shape->end.y; i++)
    {
        offset = i * 320 + shape->start.x;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;

        offset = i * 320 + shape->end.x;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }
}

/*
 * This is Base Code for Making Oval Made by D.E Kim
 * Make start x , y -> end x , y Oval
 * �ʿ��ѱ��  -> �Է¹��� ���� ó�� ���� ���ϰ�� start x, y�� ����
 *             -> �ƴҰ�� end�� x,y��ǥ�� ����Ͽ� �����Ͽ��ݴϴ�.
 *             -> ���������� ������ �׷��� Oval�� �����ݴϴ� (������ �� �����ָ� �ɵ�?)
 *             -> ( �� ����� �ٽ����� )
 */
void DrawOval(TLCD tlcdInfo, Shape *shape)
{
    /* TODO: Draw Oval */
    printf("DrawOval Executed\n");
}

/*
 * This is Base Code for Making DrawFree Made by S.H Hong
 */
void DrawFree(TLCD tlcdInfo, Shape *shape)
{
    /* ���� �� ������ ������ */
    printf("DrawFree Executed\n");

    struct ListNode *node = NULL;
    int xpos, ypos, i, offset;
    //도형 크기 동적 할당
    shape->position = (int **)malloc(sizeof(int *) * SIZEOF_CANVAS_Y); //캔버스의 y크기: 220
    for (i = 0; i < 220; i++)
    {
        shape->position[i] = (int *)malloc(sizeof(int) * SIZEOF_CANVAS_X); //캔버스의 x크기: 200
    }

    /* 터치 입력을 받음 */
    while (1)
    {
        InputTouch(&tlcdInfo);
        /*코드 구현*/
        xpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
        ypos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;
        shape->position[ypos - START_CANVAS_Y][xpos - START_CANVAS_X] = 1;
        offset = ypos * 320 + xpos;
        printf("xpos: %d\nypos: %d\n", xpos, ypos);
        *(tlcdInfo.pfbdata + offset) = BLACK;
        offset = shape->end.y * 320 + i;
        if (tlcdInfo.pressure == 0)
        {
            //Test code
            printf("xpos = %d\nypos=%d\n", xpos, ypos);
            break;
            /* position 동적 할당 */
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