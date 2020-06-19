#include "btn.h"
#include "draw.h"
#include "list.h"
#include "ui.h"

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

int GetBtn(TLCD tlcdInfo, int xpos, int ypos)
{
    int inputBtnFlag = 0;

    if ((xpos >= 270 && xpos <= 320) && (ypos >= 175 && ypos <= 205))
    {
        inputBtnFlag = TOUCH_PEN;
    }
    else if ((xpos >= 270 && xpos <= 320) && (ypos >= 210 && ypos <= 240))
    {
        inputBtnFlag = TOUCH_FILL;
    }
    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 0 && ypos <= 30))
    {
        inputBtnFlag = TOUCH_LINE;
        isFirst = 1;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 35 && ypos <= 65))
    {
        inputBtnFlag = TOUCH_RECT;
        isFirst = 1;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 70 && ypos <= 100))
    {
        inputBtnFlag = TOUCH_OVAL;
        isFirst = 1;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 105 && ypos <= 135))
    {
        inputBtnFlag = TOUCH_FREEDRAW;
        isFirst = 1;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 140 && ypos <= 170))
    {
        inputBtnFlag = TOUCH_SEL;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 175 && ypos <= 205))
    {
        inputBtnFlag = TOUCH_ERASE;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 210 && ypos <= 240))
    {
        inputBtnFlag = TOUCH_CLEAR;
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 0 && ypos <= 42))
    {
        inputBtnFlag = TOUCH_WHITE;
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 0 && ypos <= 42))
    {
        inputBtnFlag = TOUCH_ORANGE;
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 44 && ypos <= 86))
    {
        inputBtnFlag = TOUCH_RED;
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 44 && ypos <= 86))
    {
        inputBtnFlag = TOUCH_GREEN;
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 88 && ypos <= 130))
    {
        inputBtnFlag = TOUCH_YELLOW;
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 88 && ypos <= 130))
    {
        inputBtnFlag = TOUCH_NAVY;
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 132 && ypos <= 174))
    {
        inputBtnFlag = TOUCH_BLUE;
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 132 && ypos <= 174))
    {
        inputBtnFlag = TOUCH_BLACK;
    }

    else if ((xpos >= 60 && xpos <= 260) && (ypos >= 10 && ypos <= 230))
    {
        inputBtnFlag = TOUCH_CANVAS;
    }

    return inputBtnFlag;
}

void SensingTouch(TLCD tlcdInfo)
{
    int xpos, ypos, ret;
    Shape shape;
    shape.position = NULL;

    InputTouch(&tlcdInfo);

    // 보정을 넣은 lcd상의 x , y의 포지션
    xpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
    ypos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;

    // 터치가 된곳의 위치에 따라달라짐
    if (pressure >= 180)
    {
        ret = GetBtn(tlcdInfo, xpos, ypos);
    }
    else
    {
        printf("shape.start: x(%d) y(%d)\nshape.end: x(%d) y(%d)\n", shape.start.x, shape.start.y, shape.end.x, shape.end.y);
        ret = -1;
    }
    switch (ret)
    {
    case TOUCH_WHITE:
        printf("todo WhiteColor\n");
        shape.outColor = WHITE; //Set OutBound Color WHITE (why!???)
        break;
    case TOUCH_ORANGE:
        printf("todo OrangeColor\n");
        shape.outColor = ORANGE; //Set OutBound Color ORANGE
        break;
    case TOUCH_RED:
        printf("todo RedColor\n");
        shape.outColor = RED; //Set OutBound Color RED
        break;
    case TOUCH_GREEN:
        printf("todo GreenColor\n");
        shape.outColor = GREEN; //Set OutBound Color GREEN
        break;
    case TOUCH_YELLOW:
        printf("todo YellowColor\n");
        shape.outColor = YELLOW; //Set OutBound Color YELLOW
        break;
    case TOUCH_NAVY:
        printf("todo NavyColor\n");
        shape.outColor = NAVY; //Set OutBound Color NAVY
        break;
    case TOUCH_BLUE:
        printf("todo BlueColor\n");
        shape.outColor = BLUE; //Set OutBound Color BLUE
        break;
    case TOUCH_BLACK:
        printf("todo BlackColor\n");
        shape.outColor = BLACK; //Set OutBound Color BLACK
        break;
    case TOUCH_CANVAS:
        printf("touching screen %d %d\n", xpos, ypos);
        if (state >= 0 && state < 9)
        {
            shape.outColor = 0;

            // set Up Start x , y pos
            if (isFirst == 1)
            {
                shape.start.x = xpos;
                shape.start.y = ypos;

                isFirst = -1;
            }
            // set Up End x , y pos
            shape.end.x = xpos;
            shape.end.y = ypos;

            g_drawTable[state](tlcdInfo, &shape);
            struct ListNode *node = CreateNode(shape);
            Append(node);
        }
        break;
    default:
        /*
         * 터치를 때면 ret이 0이 되기 때문에
         * 항상 0이 들어오는 오류를 발견했습니다.
         * if 문으로 수정했습니다.
         */
        if (ret != -1)
        {
            state = ret;
        }
        break;
    }
}
