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

        if ((endX < START_CANVAS_X || endX > END_CANVAS_X) || (endY < START_CANVAS_Y || endY > END_CANVAS_Y)) //캔버스 범위 안에 없을 시
        {
            break;
        }

        else if (tlcdInfo->pressure == 0)
        {
            endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            break;
        }

        else
        {
            //기존에 있던 흔적 제거
            if (startX < tempX) //1, 4 사분면
            {
                incline = (double)((double)(tempY - startY) / (double)(tempX - startX)); //기울기 = y증가량 / x증가량
                yIntercept = (double)(tempY - incline * tempX);                          //y절편 = y - 기울기 * x

                for (i = startX; i <= tempX; i++)
                {
                    if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number >= 1)
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color;
                    }

                    else
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }
            }

            else //2, 3 사분면
            {
                incline = (double)((double)(tempY - startY) / (double)(tempX - startX));
                yIntercept = (double)(tempY - incline * tempX);

                for (i = startX; i >= tempX; i--)
                {
                    if (sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number == 1)
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color;
                    }

                    else
                    {
                        offset = (int)(incline * i + yIntercept) * 320 + (i);
                        *(tlcdInfo->pfbdata + offset) = WHITE;
                    }
                }
            }
            //흔적 제거 끝

            endX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            endY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;
        }

        {
            //새로운 Line 그리기
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
    }

    //sketchBook에 추가
    if (startX < endX) //1, 4 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX)); //기울기 = y증가량 / x증가량
        yIntercept = (double)(endY - incline * endX);                          //y절편 = y - 기울기 * x

        for (i = startX; i <= endX; i++)
        {
            sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
            sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;
        }
    }

    else //2, 3 사분면
    {
        incline = (double)((double)(endY - startY) / (double)(endX - startX));
        yIntercept = (double)(endY - incline * endX);

        for (i = startX; i >= endX; i--)
        {
            sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
            sketchBook[(int)(incline * i + yIntercept) - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;
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

    isFirst = 1;

    while (1) //시작지점의 x, y좌표 입력
    {
        InputTouch(tlcdInfo);

        // 처음항인가? 밑에부분 한번에 합칠수있으면 좋겠음
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
                    if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                    {
                        offset = i * 320 + j;
                        *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                    }

                    else
                    {
                        offset = i * 320 + j;
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

    shape->start.x = tempX;
    shape->start.y = tempY;

    shape->end.x = endX;
    shape->end.y = endY;

    for (i = tempX; i < endX; i++)
    {
        offset = tempY * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[tempY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
        sketchBook[tempY - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;

        offset = endY * 320 + i;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].number += 1;
        sketchBook[endY - START_CANVAS_Y][i - START_CANVAS_X].color += shape->outColor;
    }

    for (i = tempY; i < endY; i++)
    {
        offset = i * 320 + startX;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[i - START_CANVAS_Y][tempX - START_CANVAS_X].number += 1;
        sketchBook[i - START_CANVAS_Y][tempX - START_CANVAS_X].color += shape->outColor;

        offset = i * 320 + endX;
        *(tlcdInfo->pfbdata + offset) = shape->outColor;

        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].number += 1;
        sketchBook[i - START_CANVAS_Y][endX - START_CANVAS_X].color += shape->outColor;
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
            for (i = tempY - 1; i <= endY; i++)
            {
                for (j = tempX - 1; j <= endX; j++)
                {
                    if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                    {
                        offset = i * 320 + j;
                        *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                    }

                    else
                    {
                        offset = i * 320 + j;
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

        x = 0;
        y = b;

        dx = 2 * bb * x;
        dy = 2 * aa * y;

        d1 = bb - (b * aa) + (0.25 * aa);

        while (dx < dy)
        {
            offset = (y + centerY) * 320 + (x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (y + centerY) * 320 + (-x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (-y + centerY) * 320 + (x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (-y + centerY) * 320 + (-x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            ++x;
            dx += (2 * bb);

            if (d1 < 0)
            {
                d1 += (dx + bb);
            }

            else
            {
                --y;
                dy -= (2 * aa);
                d1 += (dx - dy + bb);
            }
        }

        x = a;
        y = 0;

        dx = 2 * bb * x;
        dy = 2 * aa * y;

        d2 = aa - (a * bb) + (0.25 * bb);

        while (dx > dy)
        {

            offset = (y + centerY) * 320 + (x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (y + centerY) * 320 + (-x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (-y + centerY) * 320 + (x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            offset = (-y + centerY) * 320 + (-x + centerX);
            *(tlcdInfo->pfbdata + offset) = shape->outColor;

            ++y;
            dy += (2 * aa);

            if (d2 < 0)
            {
                d2 += (dy + aa);
            }

            else
            {
                --x;
                dx -= (2 * bb);
                d2 += (dy - dx + aa);
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

    x = 0;
    y = b;

    dx = 2 * bb * x;
    dy = 2 * aa * y;

    d1 = bb - (b * aa) + (0.25 * aa);
    printf("centerX=%d\ncenterY=%d\na=%d\nb=%d\naa=%d\nbb=%d\nx=%d\ny=%d\ndx=%d\ndy=%d\nd1=%d\n", centerX, centerY, a, b, aa, bb, x, y, dx, dy, d1);

    while (dx < dy)
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

        ++x;
        dx += (2 * bb);

        if (d1 < 0)
        {
            d1 += (dx + bb);
        }

        else
        {
            --y;
            dy -= (2 * aa);
            d1 += (dx - dy + bb);
        }
    }

    x = a;
    y = 0;

    dx = 2 * bb * x;
    dy = 2 * aa * y;

    d2 = aa - (a * bb) + (0.25 * bb);

    while (dx > dy)
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

        ++y;
        dy += (2 * aa);

        if (d2 < 0)
        {
            d2 += (dy + aa);
        }

        else
        {
            --x;
            dx -= (2 * bb);
            d2 += (dy - dx + aa);
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
                sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].number += 1;             //스케치북에도 그려진 곳을 저장
                sketchBook[ypos - START_CANVAS_Y + j][xpos - START_CANVAS_X + i].color = shape->outColor; //색깔도 저장
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
        printf("Nothing happened")
    }
    int startX, startY;
    int endX, endY;
    int offset, i, j, tempX, tempY, tmp;

    //Oval 전용 변수들
    int centerX, centerY, a, b, aa, bb, x, y, dx, dy, d1, d2;

    /* touchedPointX, Y = 초반에 눌려진 좌표 
     * shape->moveX, shape->moveY = 갱신되어 눌려진 좌표
     * node->moveX, node->moveY = shape->moveX - touchedPointX, shape->moveY - touchedPointY 와 같음
     */
    int touchedPointX = shape->start.x;
    int touchedPointY = shape->start.y;
    shape->moveX = -1;
    shape->moveY = -1;
    //도형을 찾았을 경우
    if (node != NULL)
    {
        //처음 터치 된 x,y 좌표 값을 할당
        startX = node->shape.start.x;
        startY = node->shape.start.y;
        endX = node->shape.end.x;
        endY = node->shape.end.y;
        /*
         * 해당 도형에 따른 스케치북 값을 -1 해줘야 함(이동시킬 것이기 때문)
         */
        if (node->shape.type == TOUCH_LINE)
        {
        }
        else if (node->shape.type == TOUCH_RECT)
        {
        }
        else if (node->shape.type == TOUCH_OVAL)
        {
            /* set Start and end X , Y */
            centerX = (startX + endX) / 2 + node->shape.moveX;
            centerY = (startY + endY) / 2 + node->shape.moveY;

            a = (endX + node->shape.moveX - centerX); // 선 a의 길이
            b = (endY + node->shape.moveY - centerY); // 선 b의 길이
            aa = a * a;
            bb = b * b;

            x = 0;
            y = b;

            dx = 2 * bb * x;
            dy = 2 * aa * y;

            d1 = bb - (b * aa) + (0.25 * aa);
            while (dx < dy)
            {
                offset = (y + centerY) * 320 + (x + centerX);
                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;

                ++x;
                dx += (2 * bb);

                if (d1 < 0)
                {
                    d1 += (dx + bb);
                }

                else
                {
                    --y;
                    dy -= (2 * aa);
                    d1 += (dx - dy + bb);
                }
            }

            x = a;
            y = 0;

            dx = 2 * bb * x;
            dy = 2 * aa * y;

            d2 = aa - (a * bb) + (0.25 * bb);

            while (dx > dy)
            {

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[-y + centerY - START_CANVAS_Y][x + centerX - START_CANVAS_X].color = node->shape.outColor;

                *(tlcdInfo->pfbdata + offset) = node->shape.outColor;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].number -= 1;
                sketchBook[-y + centerY - START_CANVAS_Y][-x + centerX - START_CANVAS_X].color = node->shape.outColor;

                ++y;
                dy += (2 * aa);

                if (d2 < 0)
                {
                    d2 += (dy + aa);
                }

                else
                {
                    --x;
                    dx -= (2 * bb);
                    d2 += (dy - dx + aa);
                }
            }
        }
        else if (node->shape.type == TOUCH_FREEDRAW)
        {
        }

        while (1)
        {
            InputTouch(tlcdInfo);
            // 루프를 한번 돌았을때 값갱신전 초기화
            if (shape->moveX != -1 && shape->moveY != -1)
            {
                for (i = tempY + node->shape.moveY; i <= endY + node->shape.moveY; i++)
                {
                    for (j = tempX + node->shape.moveX; j <= endX + node->shape.moveX; j++)
                    {
                        if (sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number >= 1)
                        {
                            offset = i * 320 + j;
                            *(tlcdInfo->pfbdata + offset) = sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color;
                        }

                        else
                        {
                            offset = i * 320 + j;
                            *(tlcdInfo->pfbdata + offset) = WHITE;
                        }
                    }
                }
            }

            //값갱신
            tempX = startX;
            tempY = startY;

            //움직이는 값 구함
            shape->moveX = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
            shape->moveY = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

            //기존 도형에서 이동한 크기 구함
            node->shape.moveX = shape->moveX - touchedPointX;
            node->shape.moveY = shape->moveY - touchedPointY;

            if (node->shape.type == TOUCH_LINE)
            {
            }
            else if (node->shape.type == TOUCH_RECT)
            {
            }

            else if (node->shape.type == TOUCH_OVAL)
            {
                printf("START WHILE OVAL\n");

                //타원 그려주기만 함
                centerX = (tempX + endX) / 2 + node->shape.moveX; //평행이동된 값까지 더함
                centerY = (tempY + endY) / 2 + node->shape.moveY; //평행이동된 값까지 더함

                a = (endX + node->shape.moveX - centerX); // 선 a의 길이
                b = (endY + node->shape.moveY - centerY); // 선 b의 길이
                aa = a * a;
                bb = b * b;

                x = 0;
                y = b;

                dx = 2 * bb * x;
                dy = 2 * aa * y;

                d1 = bb - (b * aa) + (0.25 * aa);

                while (dx < dy)
                {
                    offset = (y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (-y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (-y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    ++x;
                    dx += (2 * bb);

                    if (d1 < 0)
                    {
                        d1 += (dx + bb);
                    }

                    else
                    {
                        --y;
                        dy -= (2 * aa);
                        d1 += (dx - dy + bb);
                    }
                }

                x = a;
                y = 0;

                dx = 2 * bb * x;
                dy = 2 * aa * y;

                d2 = aa - (a * bb) + (0.25 * bb);

                while (dx > dy)
                {

                    offset = (y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (-y + centerY) * 320 + (x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    offset = (-y + centerY) * 320 + (-x + centerX);
                    *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                    ++y;
                    dy += (2 * aa);

                    if (d2 < 0)
                    {
                        d2 += (dy + aa);
                    }

                    else
                    {
                        --x;
                        dx -= (2 * bb);
                        d2 += (dy - dx + aa);
                    }
                }
                if (tlcdInfo->pressure == 0)
                {
                    printf("PRESSURE == 0 QUOTE START\n");
                    centerX = (tempX + endX) / 2 + node->shape.moveX; //평행이동된 값까지 더함
                    centerY = (tempY + endY) / 2 + node->shape.moveY; //평행이동된 값까지 더함

                    a = (endX + node->shape.moveX - centerX); // 선 a의 길이
                    b = (endY + node->shape.moveY - centerY); // 선 b의 길이
                    aa = a * a;
                    bb = b * b;

                    x = 0;
                    y = b;

                    dx = 2 * bb * x;
                    dy = 2 * aa * y;

                    d1 = bb - (b * aa) + (0.25 * aa);

                    while (dx < dy)
                    {
                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        ++x;
                        dx += (2 * bb);

                        if (d1 < 0)
                        {
                            d1 += (dx + bb);
                        }

                        else
                        {
                            --y;
                            dy -= (2 * aa);
                            d1 += (dx - dy + bb);
                        }
                    }

                    x = a;
                    y = 0;

                    dx = 2 * bb * x;
                    dy = 2 * aa * y;

                    d2 = aa - (a * bb) + (0.25 * bb);

                    while (dx > dy)
                    {

                        offset = (y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (-y + centerY) * 320 + (x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        offset = (-y + centerY) * 320 + (-x + centerX);
                        *(tlcdInfo->pfbdata + offset) = node->shape.outColor;

                        ++y;
                        dy += (2 * aa);

                        if (d2 < 0)
                        {
                            d2 += (dy + aa);
                        }

                        else
                        {
                            --x;
                            dx -= (2 * bb);
                            d2 += (dy - dx + aa);
                        }
                    }
                    break;
                    printf("PRESSURE == 0 QUOTE END\n");
                }
            }

            else if (node->shape.type == TOUCH_FREEDRAW)
            {
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
 * TODO
 */
void DrawFill(TLCD *tlcdInfo, Shape *shape)
{
    printf("DrawFill Executed\n");

    return;
}
