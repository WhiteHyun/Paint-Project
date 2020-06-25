#include "draw.h"
#include "ui.h"
#include "list.h"
#include "btn.h"

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

// For Line ErrHandle
void DrawingOutsideCanvase(TLCD *tlcdInfo)
{
    int i, j, offset;

    for (i = START_CANVAS_X - 1; i < END_CANVAS_X + 1; i++)
    {
        offset = (START_CANVAS_Y - 1) * 320 + i;
        *(tlcdInfo->pfbdata + offset) = CYAN;

        offset = (END_CANVAS_Y)*320 + i;
        *(tlcdInfo->pfbdata + offset) = CYAN;
    }

    for (i = START_CANVAS_Y - 1; i < END_CANVAS_Y + 1; i++)
    {
        offset = i * 320 + START_CANVAS_X - 1;
        *(tlcdInfo->pfbdata + offset) = CYAN;

        offset = i * 320 + END_CANVAS_X;
        *(tlcdInfo->pfbdata + offset) = CYAN;
    }
}

/*
 * This is Base Code for Making Line Made by D.S Kim
 * Make start x , y -> end x , y Line
 */
void DrawLine(TLCD *tlcdInfo, Shape *shape)
{
    shape->type = TOUCH_LINE;

    int i, offset, isOver;
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
        isOver = 0;

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
                isOver = 1;
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
                isOver = 1;
            }
        }

        //새로운 Line 그리기
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
        } //새로운 Line 그리기 끝
        //바깥쪽 cyan 한줄
        if (isOver == 1)
        {
            DrawingOutsideCanvase(tlcdInfo);
        }
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
 */
void DrawRectangle(TLCD *tlcdInfo, Shape *shape)
{
    shape->type = TOUCH_RECT;
    int i, j, tmp, offset;

    int startX, startY, tempX, tempY, endX, endY, isFirst;

    // first value
    endX = -1;
    endY = -1;

    isFirst = 1;

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo); //터치 이벤트 처리

        // 터치가 처음 됬을 때 start값 갱신
        if (isFirst == 1)
        {
            startX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            startY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            isFirst = -1;
        }

        // 루프를 한번 돌았을때 값갱신전 초기화
        // 스케치북과 비교하여 그전에 있던 흔적(사각형)을 제거
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

        //초기화후 보여주는부분 사각형이 그려짐
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
    //터치 스크린에서 터치펜을 떼었을 때
    //shape의 start값과 end값 갱신
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
                if (shape->position[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i] != 1)
                {
                    shape->position[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i] = 1;                //프리드로우 그려진 부분을 저장
                    sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].number += 1;             //스케치북에도 그려진 곳을 저장
                    sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].color = shape->outColor; //색깔도 저장
                }
                else
                {
                    sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].color = shape->outColor;
                }
            }
        }
    }
}

/*
 * This is Base Code for Making DrawSelect Made by S.H Hong
 */
void DrawSelect(TLCD *tlcdInfo, Shape *shape)
{
    struct ListNode *node = SearchShape(shape->start.x, shape->start.y);

    if (node == NULL)
    {
        return;
    }

    int startX, startY; //도형의 Point 시작점
    int endX, endY;     //도형의 Point 끝점
    int offset, i, j;

    //Line 전용 변수들
    double incline, yIntercept; //기울기, 절편

    //Oval 전용 변수들
    int centerX, centerY, a, b, aa, bb, x, y;

    /* touchedPointX, Y = 초반에 눌려진 좌표
     * shape->moveX, shape->moveY = 갱신되어 눌려진 좌표
     * node->moveX, node->moveY = shape->moveX - touchedPointX, shape->moveY - touchedPointY 와 같음
     */
    int touchedPointX = shape->start.x;
    int touchedPointY = shape->start.y;
    shape->moveX = -1;
    shape->moveY = -1;

    //처음 터치 된 x,y 좌표 값을 할당
    startX = node->shape.start.x;
    startY = node->shape.start.y;
    endX = node->shape.end.x;
    endY = node->shape.end.y;

    /*
     * 해당 도형에 따른 스케치북 값을 -1 해줘야 함(이동시킬 것이기 때문)
     */

    //선택된 도형(선)의 sketchBook 값을 -1로 줄여 값을 없앰
    if (node->shape.type == TOUCH_LINE)
    {
        //sketchbook에서 삭제
        if (startX <= endX) //1, 4 사분면
        {
            incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
            yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

            if (startX == endX) //When incline is infinity
            {
                if (startY > endY)
                {
                    for (i = startY; i >= endY; i--)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number -= 1;

                        if (sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number >= 1)
                        {
                            sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color = node->shape.outColor;
                        }

                        else
                        {
                            sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color = WHITE;
                        }
                    }
                }

                else
                {
                    for (i = startY; i >= endY; i--)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number -= 1;

                        if (sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number >= 1)
                        {
                            sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color = node->shape.outColor;
                        }

                        else
                        {
                            sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color = WHITE;
                        }
                    }
                }
            }

            else if (incline <= -1) //4사분면 기울기 <= -1
            {
                for (i = startY; i >= endY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;

                    if (sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = WHITE;
                    }
                }
            }

            else if (incline >= 1) //1사분면 기울기 >= 1
            {
                for (i = startY; i <= endY; i++)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;

                    if (sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = WHITE;
                    }
                }
            }

            else //(-1 <= incline && incline <= 1)
            {
                for (i = startX; i <= endX; i++)
                {
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;

                    if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE;
                    }
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
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;

                    if (sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = WHITE;
                    }
                }
            }

            else if (incline >= 1) //2사분면 기울기 >= 1
            {
                for (i = startY; i >= endY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;

                    if (sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color = WHITE;
                    }
                }
            }

            else
            {
                for (i = startX; i >= endX; i--)
                {
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;

                    if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number >= 1)
                    {
                        sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                    }

                    else
                    {
                        sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE;
                    }
                }
            }
        }
    }

    //선택된 도형(사각형)의 sketchBook 값을 -1로 줄여 값을 없앰
    else if (node->shape.type == TOUCH_RECT)
    {
        //사각형 크기만큼 반복
        for (i = startX; i < endX; i++)
        {
            //값이 캔버스 내에 존재하는 경우
            if (i > START_CANVAS_X && i < END_CANVAS_X && startY > START_CANVAS_Y && startY < END_CANVAS_Y)
            {
                offset = startY * 320 + i;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                if (sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].number == 0)
                {
                    sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                }
            }
            //값이 캔버스 내에 존재하는 경우
            if (i > START_CANVAS_X && i < END_CANVAS_X && endY > START_CANVAS_Y && endY < END_CANVAS_Y)
            {
                offset = endY * 320 + i;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                if (sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number == 0) //만약 스케치북에 값이 존재하지 않으면
                {
                    sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE; //캔버스 색에 맞춤
                }
                else //그 외 값이 존재한다면 현 노드의 outColor로 설정(분명 문제 있음, Oval도 이와 구현했음)
                {
                    sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                }
            }
        }

        //사각형 크기만큼 반복
        for (i = startY; i < endY; i++)
        {
            //값이 캔버스 내에 존재하는 경우
            if (startX > START_CANVAS_X && startX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
            {
                offset = i * 320 + startX;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].number -= 1;
                if (sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].number == 0)
                {
                    sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].color = node->shape.outColor;
                }
            }

            //값이 캔버스 내에 존재하는 경우
            if (endX > START_CANVAS_X && endX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
            {
                offset = i * 320 + endX;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number -= 1;
                if (sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number == 0)
                {
                    sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = node->shape.outColor;
                }
            }
        }
    }

    //선택된 도형(타원)의 sketchBook 값을 -1로 줄여 값을 없앰
    else if (node->shape.type == TOUCH_OVAL)
    {
        /* set Start and end X , Y */
        centerX = (startX + endX) / 2;
        centerY = (startY + endY) / 2;

        a = (endX - centerX); // 선 a의 길이
        b = (endY - centerY); // 선 b의 길이

        aa = a * a;
        bb = b * b;

        for (i = startX; i <= centerX; i++)
        {
            x = i - centerX;
            for (j = startY; j <= centerY; j++)
            {
                y = j - centerY;
                if (((x * x * bb) + (y * y * aa) <= aa * bb))
                {
                    //캔버스 내에 그려져 있는 경우 그 밑 조건문도 마찬가지!
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    break;
                }
            }
        }

        for (i = startY; i <= centerY; i++)
        {
            y = i - centerY;
            for (j = startX; j <= centerX; j++)
            {
                x = j - centerX;

                if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
                {
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0) //만약 스케치북에 값이 존재하지 않으면
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE; //캔버스 색에 맞춤
                        }
                        else //그 외 값이 존재한다면 현 노드의 outColor로 설정(분명 문제 있음, Oval도 이와 구현했음)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {

                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    break;
                }
            }
        }
    }

    else if (node->shape.type == TOUCH_FREEDRAW)
    {
        for (i = 0; i < SIZEOF_CANVAS_Y; i++)
        {
            for (j = 0; j < SIZEOF_CANVAS_X; j++)
            {
                if (node->shape.position[i][j] == 1)
                {
                    if (i + node->shape.moveY >= 0 && i + node->shape.moveY < SIZEOF_CANVAS_Y &&
                        j + node->shape.moveX >= 0 && j + node->shape.moveY < SIZEOF_CANVAS_X)
                    {
                        sketchBook[i + node->shape.moveY][j + node->shape.moveX].number -= 1;
                        if (sketchBook[i + node->shape.moveY][j + node->shape.moveX].number == 0)
                        {
                            sketchBook[i + node->shape.moveY][j + node->shape.moveX].color = WHITE;
                        }
                        else
                        {
                            sketchBook[i + node->shape.moveY][j + node->shape.moveX].color = node->shape.outColor;
                        }
                    }
                }
            }
        }
    }

    while (1)
    {
        InputTouch(tlcdInfo);

        if (node->shape.type == TOUCH_LINE)
        {
            // 루프를 한번 돌았을때 값갱신전 초기화
            if (shape->moveX != -1 && shape->moveY != -1)
            {
            }

            shape->moveX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            shape->moveY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            node->shape.moveX = shape->moveX - touchedPointX;
            node->shape.moveY = shape->moveY - touchedPointY;

            if (startX <= endX) //1, 4 사분면
            {
                incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
                yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

                if (startX == endX) //When incline is infinity
                {
                    if (startY > endY)
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

            if (tlcdInfo->pressure == 0)
            {
                node->shape.start.x = startX + node->shape.moveX;
                node->shape.start.y = startY + node->shape.moveY;

                node->shape.end.x = endX + node->shape.moveX;
                node->shape.end.y = endY + node->shape.moveY;

                startX = node->shape.start.x;
                startY = node->shape.start.y;

                endX = node->shape.end.x;
                endY = node->shape.end.y;

                node->shape.moveX = 0;
                node->shape.moveY = 0;

                if (startX <= endX) //1, 4 사분면
                {
                    incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
                    yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

                    if (startX == endX) //When incline is infinity
                    {
                        if (startY > endY)
                        {
                            for (i = startY; i >= endY; i--)
                            {
                                sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number += 1;
                                sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color += shape->outColor;
                            }
                        }

                        else
                        {
                            for (i = startY; i >= endY; i--)
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

                break;
            }
        }

        else if (node->shape.type == TOUCH_RECT)
        {
            // 루프를 한번 돌았을때 값갱신전 초기화
            if (shape->moveX != -1 && shape->moveY != -1)
            {
                for (i = startY + node->shape.moveY; i <= endY + node->shape.moveY; i++)
                {
                    for (j = startX + node->shape.moveX; j <= endX + node->shape.moveX; j++)
                    {
                        offset = i * 320 + j;

                        if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                        {
                            //캔버스 위치 안에서만 그려줌
                            if (j > START_CANVAS_X && i > START_CANVAS_Y && j < END_CANVAS_X && i < END_CANVAS_Y)
                            {
                                *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                            }
                        }

                        else
                        {
                            //캔버스 위치 안에서만 그려줌
                            if (j > START_CANVAS_X && i > START_CANVAS_Y && j < END_CANVAS_X && i < END_CANVAS_Y)
                            {
                                *(tlcdInfo->pfbdata + offset) = WHITE;
                            }
                        }
                    }
                }
            }

            //움직이는 값 구함
            shape->moveX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            shape->moveY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            //기존 도형에서 이동한 크기 구함
            node->shape.moveX = shape->moveX - touchedPointX;
            node->shape.moveY = shape->moveY - touchedPointY;

            //평행이동된 시작 주소값

            //초기화후 보여주는부분
            for (i = startX + node->shape.moveX; i < endX + node->shape.moveX; i++)
            {
                if (startY + node->shape.moveY > START_CANVAS_Y &&
                    startY + node->shape.moveY < END_CANVAS_Y && i > START_CANVAS_X && i < END_CANVAS_X)
                {

                    offset = (startY + node->shape.moveY) * 320 + i;
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                }

                if (endY + node->shape.moveY > START_CANVAS_Y &&
                    endY + node->shape.moveY < END_CANVAS_Y && i > START_CANVAS_X && i < END_CANVAS_X)
                {
                    offset = (endY + node->shape.moveY) * 320 + i;
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                }
            }

            for (i = startY + node->shape.moveY; i < endY + node->shape.moveY; i++)
            {
                if (i > START_CANVAS_Y && i < END_CANVAS_Y &&
                    startX + node->shape.moveX > START_CANVAS_X && startX + node->shape.moveX < END_CANVAS_X)
                {
                    offset = i * 320 + startX + node->shape.moveX;
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                }
                if (i > START_CANVAS_Y && i < END_CANVAS_Y &&
                    endX + node->shape.moveX > START_CANVAS_X && endX + node->shape.moveX < END_CANVAS_X)
                {
                    offset = i * 320 + endX + node->shape.moveX;
                    *(tlcdInfo->pfbdata + offset) = shape->outColor;
                }
            }

            if (tlcdInfo->pressure == 0)
            {
                node->shape.start.x = startX + node->shape.moveX;
                node->shape.start.y = startY + node->shape.moveY;

                node->shape.end.x = endX + node->shape.moveX;
                node->shape.end.y = endY + node->shape.moveY;

                startX = node->shape.start.x;
                startY = node->shape.start.y;

                endX = node->shape.end.x;
                endY = node->shape.end.y;

                node->shape.moveX = 0;
                node->shape.moveY = 0;

                //값 저장
                for (i = startX; i < endX; i++)
                {
                    if (i > START_CANVAS_X && i < END_CANVAS_X && startY > START_CANVAS_Y && startY < END_CANVAS_Y)
                    {
                        offset = startY * 320 + i;
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
                        sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].color = shape->outColor;
                    }
                    if (i > START_CANVAS_X && i < END_CANVAS_X && endY > START_CANVAS_Y && endY < END_CANVAS_Y)
                    {
                        offset = endY * 320 + i;
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
                        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = shape->outColor;
                    }
                }

                for (i = startY; i < endY; i++)
                {
                    if (startX > START_CANVAS_X && startX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
                    {
                        offset = i * 320 + startX;
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].number += 1;
                        sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].color = shape->outColor;
                    }

                    if (endX > START_CANVAS_X && endX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
                    {
                        offset = i * 320 + endX;
                        *(tlcdInfo->pfbdata + offset) = shape->outColor;
                        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number += 1;
                        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = shape->outColor;
                    }
                }
                break;
            }
        }

        else if (node->shape.type == TOUCH_OVAL)
        {
            // 루프를 한번 돌았을때 값갱신전 초기화
            if (shape->moveX != -1 && shape->moveY != -1)
            {
                for (i = startY + node->shape.moveY; i <= endY + node->shape.moveY; i++)
                {
                    for (j = startX + node->shape.moveX; j <= endX + node->shape.moveX; j++)
                    {
                        offset = i * 320 + j;

                        //캔버스 위치 안에서만 그려줌
                        if (j > START_CANVAS_X && i > START_CANVAS_Y && j < END_CANVAS_X && i < END_CANVAS_Y)
                        {
                            if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
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
            }

            //움직이는 값 구함
            shape->moveX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            shape->moveY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            //기존 도형에서 이동한 크기 구함
            node->shape.moveX = shape->moveX - touchedPointX;
            node->shape.moveY = shape->moveY - touchedPointY;

            //타원 그려주기만 함
            centerX = (startX + endX) / 2 + node->shape.moveX; //평행이동된 값까지 더함
            centerY = (startY + endY) / 2 + node->shape.moveY; //평행이동된 값까지 더함

            a = (endX + node->shape.moveX - centerX); // 선 a의 길이
            b = (endY + node->shape.moveY - centerY); // 선 b의 길이

            aa = a * a;
            bb = b * b;

            for (i = startX + node->shape.moveX; i <= centerX; i++)
            {
                x = i - centerX;

                for (j = startY + node->shape.moveY; j <= centerY; j++)
                {
                    y = j - centerY;

                    if (((x * x * bb) + (y * y * aa) <= aa * bb))
                    {
                        if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                            (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                        {
                            offset = (y + centerY) * 320 + (x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                            (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                        {
                            offset = (y + centerY) * 320 + (-x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                            (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                        {
                            offset = (-y + centerY) * 320 + (x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                            (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                        {
                            offset = (-y + centerY) * 320 + (-x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        break;
                    }
                }
            }

            for (i = startY + node->shape.moveY; i <= centerY; i++)
            {
                y = i - centerY;

                for (j = startX + node->shape.moveX; j <= centerX; j++)
                {
                    x = j - centerX;

                    if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
                    {
                        if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                            (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                        {
                            offset = (y + centerY) * 320 + (x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                            (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                        {
                            offset = (y + centerY) * 320 + (-x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                            (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                        {
                            offset = (-y + centerY) * 320 + (x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                            (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                        {
                            offset = (-y + centerY) * 320 + (-x + centerX);
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }

                        break;
                    }
                }
            }

            if (tlcdInfo->pressure == 0)
            {
                printf("PRESSURE == 0 QUOTE START\n");

                node->shape.start.x = startX + node->shape.moveX;
                node->shape.start.y = startY + node->shape.moveY;

                node->shape.end.x = endX + node->shape.moveX;
                node->shape.end.y = endY + node->shape.moveY;

                startX = node->shape.start.x;
                startY = node->shape.start.y;

                endX = node->shape.end.x;
                endY = node->shape.end.y;

                node->shape.moveX = 0;
                node->shape.moveY = 0;

                centerX = (startX + endX) / 2;
                centerY = (startY + endY) / 2;

                a = (endX - centerX); // 선 a의 길이
                b = (endY - centerY); // 선 b의 길이

                aa = a * a;
                bb = b * b;

                for (i = startX; i <= centerX; i++)
                {
                    x = i - centerX;

                    for (j = startY; j <= centerY; j++)
                    {
                        y = j - centerY;

                        if (((x * x * bb) + (y * y * aa) <= aa * bb))
                        {
                            if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                                (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                            {
                                offset = (y + centerY) * 320 + (x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                                (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                            {
                                offset = (y + centerY) * 320 + (-x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                                (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                            {
                                offset = (-y + centerY) * 320 + (x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                                (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                            {
                                offset = (-y + centerY) * 320 + (-x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            break;
                        }
                    }
                }

                for (i = startX; i <= centerY; i++)
                {
                    y = i - centerY;

                    for (j = startX; j <= centerX; j++)
                    {
                        x = j - centerX;

                        if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
                        {
                            if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                                (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                            {
                                offset = (y + centerY) * 320 + (x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                                (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                            {
                                offset = (y + centerY) * 320 + (-x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                                (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                            {
                                offset = (-y + centerY) * 320 + (x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                                (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                            {
                                offset = (-y + centerY) * 320 + (-x + centerX);
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number += 1;
                                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = shape->outColor;
                            }

                            break;
                        }
                    }
                }

                printf("PRESSURE == 0 QUOTE END\n");
                break;
            }
        }

        else if (node->shape.type == TOUCH_FREEDRAW)
        {
            for (i = node->shape.moveY; i < node->shape.moveY + SIZEOF_CANVAS_Y; i++)
            {
                for (j = node->shape.moveX; j < node->shape.moveX + SIZEOF_CANVAS_X; j++)
                {
                    offset = (i + START_CANVAS_Y) * 320 + j + START_CANVAS_X;

                    if (i >= 0 && i < SIZEOF_CANVAS_Y && j >= 0 && j < SIZEOF_CANVAS_X)
                    {
                        if (sketchBook[i][j].number >= 1)
                        {
                            *(tlcdInfo->pfbdata + offset) = sketchBook[i][j].color;
                        }

                        else
                        {
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }
            }

            //움직이는 값 구함
            shape->moveX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            shape->moveY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            //기존 도형에서 이동한 크기 구함
            node->shape.moveX += shape->moveX - touchedPointX;
            node->shape.moveY += shape->moveY - touchedPointY;

            //프리드로우는 다르게 계산되어야 하기 때문에 초기 터치값도 갱신함
            touchedPointX = shape->moveX;
            touchedPointY = shape->moveY;

            printf("node->shape.moveX = %d\n", node->shape.moveX);
            printf("node->shape.moveY = %d\n", node->shape.moveY);

            //움직인 값에 따라 그림을 그려줌
            for (i = node->shape.moveY; i < node->shape.moveY + SIZEOF_CANVAS_Y; i++)
            {
                for (j = node->shape.moveX; j < node->shape.moveX + SIZEOF_CANVAS_X; j++)
                {
                    offset = (i + START_CANVAS_Y) * 320 + j + START_CANVAS_X;

                    if (i >= 0 && i < SIZEOF_CANVAS_Y && j >= 0 && j < SIZEOF_CANVAS_X)
                    {
                        if (node->shape.position[i - node->shape.moveY][j - node->shape.moveX] == 1)
                        {
                            *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                        }
                    }
                }
            }

            if (tlcdInfo->pressure == 0)
            {
                //기존 도형에서 이동한 크기 구함
                node->shape.moveX = shape->moveX - touchedPointX;
                node->shape.moveY = shape->moveY - touchedPointY;

                //움직인 값에 따라 그림을 그려줌
                for (i = node->shape.moveY; i < node->shape.moveY + SIZEOF_CANVAS_Y; i++)
                {
                    for (j = node->shape.moveX; j < node->shape.moveX + SIZEOF_CANVAS_X; j++)
                    {
                        offset = (i + START_CANVAS_Y) * 320 + j + START_CANVAS_X;

                        if (i >= 0 && i < SIZEOF_CANVAS_Y && j >= 0 && j < SIZEOF_CANVAS_X)
                        {
                            if (node->shape.position[i - node->shape.moveY][j - node->shape.moveX] == 1)
                            {
                                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                                ++sketchBook[i][j].number;
                                sketchBook[i][j].color = node->shape.outColor;
                            }
                        }
                    }
                }

                break;
            }
        }
    }

    return;
}

/*
 * TODO
 */
void DrawErase(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawErase Executed\n");

    struct ListNode *node = SearchShape(shape->start.x, shape->start.y);
    if (node == NULL)
    {
        return;
    }
    while (1)
    {
        InputTouch(tlcdInfo);
        if (tlcdInfo->pressure == 0)
        {
            tlcdInfo->pressure = -1;
            break;
        }
    }

    double incline, yIntercept; //Line 전용 변수

    int startX, startY; //도형의 Point 시작점
    int endX, endY;     //도형의 Point 끝점
    int offset, i, j;

    //Oval 전용 변수들
    int centerX, centerY, a, b, aa, bb, x, y;

    /* touchedPointX, Y = 초반에 눌려진 좌표
     * shape->moveX, shape->moveY = 갱신되어 눌려진 좌표
     * node->moveX, node->moveY = shape->moveX - touchedPointX, shape->moveY - touchedPointY 와 같음
     */
    int touchedPointX = shape->start.x;
    int touchedPointY = shape->start.y;

    shape->moveX = -1;
    shape->moveY = -1;

    //처음 터치 된 x,y 좌표 값을 할당
    startX = node->shape.start.x;
    startY = node->shape.start.y;
    endX = node->shape.end.x;
    endY = node->shape.end.y;

    /*
         * 해당 도형에 따른 스케치북 값을 -1 해줘야 함(삭제)
         */
    //선택된 도형(선)의 sketchBook 값을 -1로 줄여 값을 없앰
    if (node->shape.type == TOUCH_LINE)
    {
        //색 삭제
        if (startX <= endX) //1, 4 사분면
        {
            incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
            yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

            if (startX == endX) //When incline is infinity
            {
                if (startY > endY)
                {
                    for (i = startY; i >= endY; i--)
                    {
                        offset = (int)(i)*320 + (int)(endX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }

                else
                {
                    for (i = startY; i <= endY; i++)
                    {
                        offset = (int)(i)*320 + (int)(endX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }
            }

            else if (incline <= -1) //4사분면 기울기 <= -1
            {
                for (i = startY; i >= endY; i--)
                {
                    offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                    *(tlcdInfo->pfbdata + offset) = WHITE;
                }
            }

            else if (incline >= 1) //1사분면 기울기 >= 1
            {
                for (i = startY; i <= endY; i++)
                {
                    offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                    *(tlcdInfo->pfbdata + offset) = WHITE;
                }
            }

            else //(-1 <= incline && incline <= 1) //1, 2사분면에서 |기울기| <= 1일 때
            {
                for (i = startX; i <= endX; i++)
                {
                    offset = (int)(incline * i + yIntercept) * 320 + (i);
                    *(tlcdInfo->pfbdata + offset) = WHITE;
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
                    *(tlcdInfo->pfbdata + offset) = WHITE;
                }
            }

            else if (incline >= 1) //1사분면 기울기 >= 1
            {
                for (i = startY; i >= endY; i--)
                {
                    offset = (int)(i)*320 + (int)(i / incline - yIntercept / incline);
                    *(tlcdInfo->pfbdata + offset) = WHITE;
                }
            }

            else
            {
                for (i = startX; i >= endX; i--)
                {
                    offset = (int)(incline * i + yIntercept) * 320 + (i);
                    *(tlcdInfo->pfbdata + offset) = WHITE;
                }
            }
        }

        //sketchbook에서 삭제
        if (startX <= endX) //1, 4 사분면
        {
            incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
            yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

            if (startX == endX) //When incline is infinity
            {
                if (startY > endY)
                {
                    for (i = startY; i >= endY; i--)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number -= 1;
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color -= shape->outColor;
                    }
                }

                else
                {
                    for (i = startY; i >= endY; i--)
                    {
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].number -= 1;
                        sketchBook[i - START_CANVAS_Y][(int)(endX)-START_CANVAS_X].color -= shape->outColor;
                    }
                }
            }

            else if (incline <= -1) //4사분면 기울기 <= -1
            {
                for (i = startY; i >= endY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color -= shape->outColor;
                }
            }

            else if (incline >= 1) //1사분면 기울기 >= 1
            {
                for (i = startY; i <= endY; i++)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color -= shape->outColor;
                }
            }

            else //(-1 <= incline && incline <= 1)
            {
                for (i = startX; i <= endX; i++)
                {
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color -= shape->outColor;
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
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color -= shape->outColor;
                }
            }

            else if (incline >= 1) //2사분면 기울기 >= 1
            {
                for (i = startY; i >= endY; i--)
                {
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].number -= 1;
                    sketchBook[i - START_CANVAS_Y][(int)(i / incline - yIntercept / incline) - START_CANVAS_X].color -= shape->outColor;
                }
            }

            else
            {
                for (i = startX; i >= endX; i--)
                {
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                    sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color -= shape->outColor;
                }
            }
        }
    }

    else if (node->shape.type == TOUCH_RECT)
    {
        //사각형 크기만큼 반복
        for (i = startX; i < endX; i++)
        {
            //값이 캔버스 내에 존재하는 경우
            if (i > START_CANVAS_X && i < END_CANVAS_X && startY > START_CANVAS_Y && startY < END_CANVAS_Y)
            {
                offset = startY * 320 + i;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                if (sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].number == 0)
                {
                    sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[startY - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                }
            }
            //값이 캔버스 내에 존재하는 경우
            if (i > START_CANVAS_X && i < END_CANVAS_X && endY > START_CANVAS_Y && endY < END_CANVAS_Y)
            {
                offset = endY * 320 + i;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number -= 1;
                if (sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number == 0) //만약 스케치북에 값이 존재하지 않으면
                {
                    sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = WHITE; //캔버스 색에 맞춤
                }
                else //그 외 값이 존재한다면 현 노드의 outColor로 설정(분명 문제 있음, Oval도 이와 구현했음)
                {
                    sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color = node->shape.outColor;
                }
            }
        }

        //사각형 크기만큼 반복
        for (i = startY; i < endY; i++)
        {
            //값이 캔버스 내에 존재하는 경우
            if (startX > START_CANVAS_X && startX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
            {
                offset = i * 320 + startX;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].number -= 1;
                if (sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].number == 0)
                {
                    sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[i - START_CANVAS_Y][startX - START_CANVAS_X].color = node->shape.outColor;
                }
            }

            //값이 캔버스 내에 존재하는 경우
            if (endX > START_CANVAS_X && endX < END_CANVAS_X && i > START_CANVAS_Y && i < END_CANVAS_Y)
            {
                offset = i * 320 + endX;
                *(tlcdInfo->pfbdata + offset) = WHITE;
                sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number -= 1;
                if (sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number == 0)
                {
                    sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = WHITE;
                }
                else
                {
                    sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color = node->shape.outColor;
                }
            }
        }
    }

    else if (node->shape.type == TOUCH_OVAL)
    {
        centerX = (startX + endX) / 2;
        centerY = (startY + endY) / 2;

        a = (endX - centerX); // 선 a의 길이
        b = (endY - centerY); // 선 b의 길이

        aa = a * a;
        bb = b * b;

        for (i = startX; i <= centerX; i++)
        {
            x = i - centerX;
            for (j = startY; j <= centerY; j++)
            {
                y = j - centerY;
                if (((x * x * bb) + (y * y * aa) <= aa * bb))
                {
                    //캔버스 내에 그려져 있는 경우 그 밑 조건문도 마찬가지!
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    break;
                }
            }
        }

        for (i = startY; i <= centerY; i++)
        {
            y = i - centerY;
            for (j = startX; j <= centerX; j++)
            {
                x = j - centerX;

                if (((x * x) * bb) + ((y * y) * aa) <= aa * bb)
                {
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0) //만약 스케치북에 값이 존재하지 않으면
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE; //캔버스 색에 맞춤
                        }
                        else //그 외 값이 존재한다면 현 노드의 outColor로 설정(분명 문제 있음, Oval도 이와 구현했음)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }
                    if ((y + centerY) < END_CANVAS_Y && (y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {

                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (x + centerX) < END_CANVAS_X && (x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    if ((-y + centerY) < END_CANVAS_Y && (-y + centerY) > START_CANVAS_Y &&
                        (-x + centerX) < END_CANVAS_X && (-x + centerX) > START_CANVAS_X)
                    {
                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                        sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                        if (sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number == 0)
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = WHITE;
                        }
                        else
                        {
                            sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;
                        }
                    }

                    break;
                }
            }
        }
    }

    DeleteNode(Pop());

    return;
}

/*
 * This is Base Code for Making DrawClear Made by S.H Hong
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
 * TODO shape.inColor -> color
 */

void flood_fill(int x, int y, unsigned short firstColor, unsigned short targertColor, TLCD *tlcdInfo);

void DrawFill(TLCD *tlcdInfo, Shape *shape)
{

    printf("DrawFill Executed\n");

    int xpos, ypos, i, j, offset;
    unsigned short firstColor;
    // x , y position

    xpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
    ypos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

    if (xpos > START_CANVAS_X && xpos < END_CANVAS_X && ypos > START_CANVAS_Y && ypos < END_CANVAS_Y)
    {
        if (sketchBook[ypos - START_CANVAS_Y][xpos - START_CANVAS_X].number >= 1)
        {
            firstColor = sketchBook[ypos - START_CANVAS_Y][xpos - START_CANVAS_X].color;
        }
        else
        {
            firstColor = WHITE;
        }

        // TODO COLOR
        flood_fill(xpos - START_CANVAS_X, ypos - START_CANVAS_Y, firstColor, shape->inColor, tlcdInfo);
    }

    return;
}

void flood_fill(int x, int y, unsigned short firstColor, unsigned short targertColor, TLCD *tlcdInfo)
{
    int offset;
    if (x > 0 && x < 200 && y > 0 && y < 220)
    {
        if (sketchBook[y][x].color == firstColor) // read_pixel 함수 호출의 결과로 0을 발견하면
        {
            offset = ((y + START_CANVAS_Y) * 320) + x + START_CANVAS_X;
            *(tlcdInfo->pfbdata + offset) = targertColor;
            sketchBook[y][x].color = targertColor;

            flood_fill(x - 1, y, firstColor, targertColor, tlcdInfo); // 순환호출(x-1번째 위치)
            flood_fill(x + 1, y, firstColor, targertColor, tlcdInfo); // 순환호출(x+1번째 위치)
            flood_fill(x, y - 1, firstColor, targertColor, tlcdInfo); // 순환호출(y-1번째 위치)
            flood_fill(x, y + 1, firstColor, targertColor, tlcdInfo); // 순환호출(y+1번째 위치)
        }
    }
}
