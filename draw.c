#include "draw.h"
#include "ui.h"
#include "list.h"
#include "btn.h"
#include <stdio.h>
#include <stdlib.h>

extern struct List *g_List;

/*
 * 논의사항 : 러버밴드.
 * 이전에 그려진 그림이 있던자리의 정보를 가지고있는 배열
 * 러버밴드를 구현할때 이전에 그림이 그려지지 않은 위치에 그려졌을시 ( number == 0 ) -> 해당자리를 그냥 white로 변경해주고
 * 새 도화지가 아닐경우 (nuber == 1) 해당 색상을 출력해주는 함수를 만들어줍시다.
 * pressure가 0가될때 sketchBook 배열의 값을 변경하여 갱신해줌
 */

struct Pixel
{
    int number;
    unsigned short color;
};

struct Pixel sketchBook[SIZEOF_CANVAS_Y][SIZEOF_CANVAS_X];

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
    int tempX, tempY;
    double incline;    //기울기
    double yIntercept; //y절편

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (tlcdInfo->pressure != 0)
        {
            startX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            if ((startX < START_CANVAS_X || startX > END_CANVAS_X) || (startY < START_CANVAS_Y || startY > END_CANVAS_Y)) //캔버스 범위 안에 없을 시
            {
                return;
            }

            break;
        }
    }

    tlcdInfo->pressure = -1;

    tempX = startX;
    tempY = startY;

    while (1) //Line Rubber Band 구현
    {
        InputTouch(tlcdInfo);

        if (tempX != startX || tempY != startY)
        {
            tempX = endX;
            tempY = endY;
        }

        endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

        if (tempX == startX && tempY == startY)
        {
            tempX = endX;
            tempY = endY;
        }

        if (tlcdInfo->pressure == 0)
        {

            endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            if ((endX < START_CANVAS_X || endX > END_CANVAS_X) || (endY < START_CANVAS_Y || endY > END_CANVAS_Y))
            {
                if (endX < START_CANVAS_X)
                {
                    endX = START_CANVAS_X + 1;
                }
                else if (endX > END_CANVAS_X)
                {
                    endX = END_CANVAS_X - 1;
                }

                if (endY < START_CANVAS_Y)
                {
                    endY = START_CANVAS_Y + 1;
                }

                else if (endY > END_CANVAS_Y)
                {
                    endY = END_CANVAS_Y - 1;
                }
            }

            break;
        }

        else
        {
            //기존에 있던 흔적 제거
            if (startX <= tempX) //1, 4 사분면
            {
                incline = (double)((double)(tempY - startY) / (double)(tempX - startX)); //기울기 = y증가량 / x증가량
                yIntercept = (double)(tempY - incline * tempX);                          //y절편 = y - 기울기 * x

                if (startX == tempX) //When incline is infinity
                {
                    if (startY > tempY)
                    {
                        for (i = startY; i >= tempY; i--)
                        {
                            if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(tempX)-START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                            {
                                offset = (int)(i)*320 + (int)(tempX);
                                *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(tempX)-START_CANVAS_X].color;
                            }

                            else //없다면
                            {
                                offset = (int)(i)*320 + (int)(tempX);
                                *(tlcdInfo->pfbdata + offset) = WHITE;
                            }
                        }
                    }

                    else
                    {
                        for (i = startY; i <= tempY; i++)
                        {
                            if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(tempX)-START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                            {
                                offset = (int)(i)*320 + (int)(tempX);
                                *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(tempX)-START_CANVAS_X].color;
                            }

                            else //없다면
                            {
                                offset = (int)(i)*320 + (int)(tempX);
                                *(tlcdInfo->pfbdata + offset) = WHITE;
                            }
                        }
                    }
                }

                else if (incline <= -1) //4사분면에서 기울기 <= -1
                {
                    for (i = startY; i >= tempY; i--)
                    {
                        if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }

                else if (incline >= 1) //1사분면에서 기울기 >= 1
                {
                    for (i = startY; i <= tempY; i++)
                    {
                        if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }

                else //(-1 <= incline && incline <= 1) //1, 2사분면에서 |기울기| <= 1일 때
                {
                    for (i = startX; i <= tempX; i++)
                    {
                        if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(incline * i + yIntercept) * 320 + (i);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(incline * i + yIntercept) * 320 + (i);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }
            }

            else //2, 3 사분면
            {
                incline = (double)((double)(tempY - startY) / (double)(tempX - startX));
                yIntercept = (double)(tempY - incline * tempX);

                if (incline <= -1) //3사분면에서 기울기 <= -1
                {
                    for (i = startY; i <= tempY; i++)
                    {
                        if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }

                else if (incline >= 1) //2사분면에서 기울기 >= 1
                {
                    for (i = startY; i >= tempY; i--)
                    {
                        if (sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(i)-START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }

                else
                {
                    for (i = startX; i >= tempX; i--)
                    {
                        if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number == 1) //스케치북에 그림이 이미 있다면
                        {
                            offset = (int)(incline * i + yIntercept) * 320 + (i);
                            *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color;
                        }

                        else //없다면
                        {
                            offset = (int)(incline * i + yIntercept) * 320 + (i);
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }
            }

            //흔적 제거 끝

            endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            if ((endX < START_CANVAS_X || endX > END_CANVAS_X) || (endY < START_CANVAS_Y || endY > END_CANVAS_Y))
            {
                if (endX < START_CANVAS_X)
                {
                    endX = START_CANVAS_X + 1;
                }
                else if (endX > END_CANVAS_X)
                {
                    endX = END_CANVAS_X - 1;
                }

                if (endY < START_CANVAS_Y)
                {
                    endY = START_CANVAS_Y + 1;
                }

                else if (endY > END_CANVAS_Y)
                {
                    endY = END_CANVAS_Y - 1;
                }
            }
        }

        {                       //새로운 Line 그리기
            if (startX <= endX) //1, 4 사분면
            {
                incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
                yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

                if (startX == endX) //When incline is infinity
                {
                    if (startY > tempY)
                    {
                        for (i = startY; i >= endY; i--)
                        {
                            offset = (int)(i)*320 + (int)(endX);
                            *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        }
                    }

                    else
                    {
                        for (i = startY; i <= endY; i++)
                        {
                            offset = (int)(i)*320 + (int)(endX);
                            *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        }
                    }
                }

                else if (incline <= -1) //4사분면 기울기 <= -1
                {
                    for (i = startY; i >= endY; i--)
                    {
                        offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }

                else if (incline >= 1) //1사분면 기울기 >= 1
                {
                    for (i = startY; i <= endY; i++)
                    {
                        offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }

                else //(-1 <= incline && incline <= 1) //1, 2사분면에서 |기울기| <= 1일 때
                {
                    for (i = startX; i <= endX; i++)
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }
            }

            else //2, 3 사분면
            {
                incline = (double)((double)(endY - startY) / (double)(endX - startX));
                yIntercept = (double)(endY - incline * endX);

                if (incline <= -1) //4사분면 기울기 <= -1
                {
                    for (i = startY; i <= endY; i++)
                    {
                        offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }

                else if (incline >= 1) //1사분면 기울기 >= 1
                {
                    for (i = startY; i >= endY; i--)
                    {
                        offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }

                else
                {
                    for (i = startX; i >= endX; i--)
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    }
                }
            }
        } //새로운 Line 그리기 끝
    }

    //sketchBook에 추가
    if (startX <= endX) //1, 4 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
        yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

        if (startX == endX) //When incline is infinity
        {
            if (startY > tempY)
            {
                for (i = startY; i >= tempY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number += 1;
                    sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color += shape->outColor;
                }
            }

            else
            {
                for (i = startY; i >= tempY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number += 1;
                    sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color += shape->outColor;
                }
            }
        }

        else if (incline <= -1) //4사분면 기울기 <= -1
        {
            for (i = startY; i >= endY; i--)
            {
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number += 1;
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color += shape->outColor;
            }
        }

        else if (incline >= 1) //1사분면 기울기 >= 1
        {
            for (i = startY; i <= endY; i++)
            {
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number += 1;
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color += shape->outColor;
            }
        }

        else //(-1 <= incline && incline <= 1)
        {
            for (i = startX; i <= endX; i++)
            {
                sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
                sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;
            }
        }
    }

    else //2, 3 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX));
        yIntercept = (double)(endY - incline * endX);

        if (incline <= -1) //3사분면 기울기 <= -1
        {
            for (i = startY; i <= endY; i++)
            {
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number += 1;
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color += shape->outColor;
            }
        }

        else if (incline >= 1) //2사분면 기울기 >= 1
        {
            for (i = startY; i >= endY; i--)
            {
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number += 1;
                sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color += shape->outColor;
            }
        }

        else
        {
            for (i = startX; i >= endX; i--)
            {
                sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
                sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;
            }
        }
    }

    shape->start.x = startX;
    shape->start.y = startY;

    shape->end.x = endX;
    shape->end.y = endY;
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
    int i, j, tmp, offset;

    int startX, startY, tempX, tempY, endX, endY, isFirst;

    // first value
    endX = -1;
    endY = -1;

    isFirst = 1;//점이 처음 찍혔는지 알려주는 변수

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        // 점이 처음 찍혔을때 start값 갱신
        if (isFirst == 1)
        {
            startX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            isFirst = -1;
        }

        // 루프를 한번 돌았을때 값갱신전 초기화
        // 그전에 있던 사각형을 지움
        if (endX != -1 && endY != -1)
        {
            for (i = tempY; i <= endY; i++)
            {
                for (j = tempX; j <= endX; j++)
                {
                    offset = i * 320 + j;

                    if (i >= END_CANVAS_Y || j >= END_CANVAS_X)
                    {
                        *(tlcdInfo->pfbdata + offset) = CYAN;
                        break;
                    }

                    else if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                    {
                        *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                    }

                    else
                    {
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }
            }
        }

        //값갱신
        tempX = startX;
        tempY = startY;

        endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

        // CANVAS의 포지션이 벗어나면 continue
        if ((endX <= START_CANVAS_X || endX >= END_CANVAS_X) || (endY <= START_CANVAS_Y || endY >= END_CANVAS_Y))
        {
            if (endX <= START_CANVAS_X)
            {
                endX = START_CANVAS_X + 1;
            }
            else if (endX >= END_CANVAS_X)
            {
                endX = END_CANVAS_X - 2;
            }

            if (endY <= START_CANVAS_Y)
            {
                endY = START_CANVAS_Y + 1;
            }

            else if (endY >= END_CANVAS_Y)
            {
                endY = END_CANVAS_Y - 2;
            }
        }

        // start , end Pos Setting
        //start의 x,y값들이 end의 x,y값들보다 크면 x,y값을 교환
        if (tempX > endX)
        {
            tmp = tempX;
            tempX = endX;
            endX = tmp;
        }

        if (tempY > endY)
        {
            tmp = tempY;
            tempY = endY;
            endY = tmp;
        }

        //초기화후 보여주는부분
        for (i = tempX; i < endX; i++)
        {
            offset = tempY * 320 + i;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = endY * 320 + i;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
        }

        for (i = tempY; i < endY; i++)
        {
            offset = i * 320 + tempX;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = i * 320 + endX;
            *(tlcdInfo->pfbdata + offset) = shape->outColor;
        }

        if (tlcdInfo->pressure == 0)
        {
            break;
        }
    }
    //터치 스크린에서 펜을 떼었을 때
    shape->start.x = tempX;
    shape->start.y = tempY;

    shape->end.x = endX;
    shape->end.y = endY;
    //sketchBook에 추가
    for (i = tempX; i < endX; i++)
    {
        offset = tempY * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[tempY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
        sketchBook[tempY - START_CANVAS_Y][i - START_CANVAS_X].color = shape->outColor;

        offset = endY * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = shape->outColor;
    }

    for (i = tempY; i < endY; i++)
    {
        offset = i * 320 + startX;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[i - START_CANVAS_Y][tempX - START_CANVAS_X].number += 1;
        sketchBook[i - START_CANVAS_Y][tempX - START_CANVAS_X].color = shape->outColor;

        offset = i * 320 + endX;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number += 1;
        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = shape->outColor;
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
    int i, j, tmp, centerX, centerY, a, b, offset, x, y, isFirst;
    int startX, startY, endX, endY, tempX, tempY;
    int aa, bb, dx, dy, d1, d2;

    isFirst = 1;

    endX = -1;
    endY = -1;

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        if (isFirst == 1)
        {
            startX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            isFirst = -1;
        }
        // 루프를 한번 돌았을때 값갱신전 초기화
        if (endX != -1 && endY != -1)
        {
            for (i = tempY; i <= endY; i++)
            {
                for (j = tempX; j <= endX; j++)
                {
                    offset = i * 320 + j;

                    if (i >= END_CANVAS_Y || j >= END_CANVAS_X)
                    {
                        *(tlcdInfo->pfbdata + offset) = CYAN;
                        break;
                    }
                    else if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                    {
                        *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                    }

                    else
                    {
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }
            }
        }
        //값갱신
        tempX = startX;
        tempY = startY;

        endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

        // CANVAS의 포지션이 벗어나면 continue
        if ((endX <= START_CANVAS_X || endX >= END_CANVAS_X) || (endY <= START_CANVAS_Y || endY >= END_CANVAS_Y))
        {
            if (endX <= START_CANVAS_X)
            {
                endX = START_CANVAS_X + 1;
            }
            else if (endX >= END_CANVAS_X)
            {
                endX = END_CANVAS_X - 2;
            }

            if (endY <= START_CANVAS_Y)
            {
                endY = START_CANVAS_Y + 1;
            }

            else if (endY >= END_CANVAS_Y)
            {
                endY = END_CANVAS_Y - 2;
            }
        }

        // start , end Pos Setting
        if (tempX > endX)
        {
            tmp = tempX;
            tempX = endX;
            endX = tmp;
        }

        if (tempY > endY)
        {
            tmp = tempY;
            tempY = endY;
            endY = tmp;
        }

        /* set Start and end X , Y */
        centerX = (tempX + endX) / 2;
        centerY = (tempY + endY) / 2;

        a = (endX - centerX); // 선 a의 길이
        b = (endY - centerY); // 선 b의 길이

        aa = a * a;
        bb = b * b;

        for (i = tempX; i <= centerX; i++)
        {
            x = i - centerX;
            for (j = tempY; j <= centerY; j++)
            {
                y = j - centerY;

                if (((x * x * bb) + (y * y * aa) <= aa * bb))
                {
                    offset = (y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (-y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (-y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    break;
                }
            }
        }

        for (i = tempY; i <= centerY; i++)
        {
            y = i - centerY;
            for (j = tempX; j <= centerX; j++)
            {
                x = j - centerX;

                if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
                {
                    offset = (y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (-y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;

                    offset = (-y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                    break;
                }
            }
        }

        if (tlcdInfo->pressure == 0)
        {
            break;
        }
    }

    shape->start.x = tempX;
    shape->start.y = tempY;

    shape->end.x = endX;
    shape->end.y = endY;

    /* set Start and end X , Y */
    centerX = (tempX + endX) / 2;
    centerY = (tempY + endY) / 2;

    a = (endX - centerX); // 선 a의 길이
    b = (endY - centerY); // 선 b의 길이

    aa = a * a;
    bb = b * b;

    for (i = tempX; i <= centerX; i++)
    {
        x = i - centerX;
        for (j = tempY; j <= centerY; j++)
        {
            y = j - centerY;

            if (((x * x * bb) + (y * y * aa) <= aa * bb))
            {
                offset = (y + centerY) * 320 + (x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (y + centerY) * 320 + (-x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (-y + centerY) * 320 + (x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (-y + centerY) * 320 + (-x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;

                break;
            }
        }
    }

    for (i = tempY; i <= centerY; i++)
    {
        y = i - centerY;
        for (j = tempX; j <= centerX; j++)
        {
            x = j - centerX;

            if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
            {
                offset = (y + centerY) * 320 + (x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (y + centerY) * 320 + (-x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (-y + centerY) * 320 + (x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;

                offset = (-y + centerY) * 320 + (-x + centerX);
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;

                break;
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
    int xpos, ypos, i, j, offset;

    //도형 크기 동적 할당
    shape->position = (int **)malloc(sizeof(int *) * SIZEOF_CANVAS_Y); //캔버스의 y크기: 220

    for (i = 0; i < SIZEOF_CANVAS_Y; i++)
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

        xpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
        ypos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

        //터치된 부분에서 3x3만큼 그려줌
        for (i = -1; i < 2; i++)
        {
            for (j = -1; j < 2; j++)
            {
                offset = (ypos + j) * tlcdInfo->fbvar.xres + xpos + i;
                //캠버스 화면을 벗어나는 경우
                if (ypos - START_CANVAS_Y + j < 0 || ypos - START_CANVAS_Y + j >= SIZEOF_CANVAS_Y ||
                    xpos - START_CANVAS_X + i < 0 || xpos - START_CANVAS_X + i >= SIZEOF_CANVAS_X)
                {
                    continue;
                }
                *(tlcdInfo->pfbdata + offset) = shape->outColor;
                shape->position[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i] = 1;                //프리드로우 그려진 부분을 저장
                sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].number = 1;              //스케치북에도 그려진 곳을 저장
                sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].color = shape->outColor; //색깔도 저장
            }
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
            sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number = 0;
            sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color = WHITE;

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
