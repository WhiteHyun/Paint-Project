#include "btn.h"
#include "draw.h"
#include "list.h"
#include "ui.h"

extern struct List *g_List;

inline void InputTouch(TLCD *tlcdInfo)
{
    read(tlcdInfo->fd, &tlcdInfo->ie, sizeof(struct input_event));

    if (tlcdInfo->ie.type == 3)
    {
        if (tlcdInfo->ie.code == 0)
        {
            tlcdInfo->x = tlcdInfo->ie.value;
            // printf("[Before]tlcdInfo->x = %d\n", tlcdInfo->x);
        }

        else if (tlcdInfo->ie.code == 1)
        {
            tlcdInfo->y = tlcdInfo->ie.value;
            // printf("[Before]tlcdInfo->y = %d\n", tlcdInfo->y);
        }

        else if (tlcdInfo->ie.code == 24)
        {
            tlcdInfo->pressure = tlcdInfo->ie.value;
            // printf("[Before]tlcdInfo->pressure = %d\n", tlcdInfo->pressure);
        }
    }
}

int GetBtn(int xpos, int ypos)
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
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 35 && ypos <= 65))
    {
        inputBtnFlag = TOUCH_RECT;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 70 && ypos <= 100))
    {
        inputBtnFlag = TOUCH_OVAL;
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 105 && ypos <= 135))
    {
        inputBtnFlag = TOUCH_FREEDRAW;
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

void SensingTouch(TLCD *tlcdInfo)
{
    int xpos, ypos, ret;
    Shape shape;
    shape.position = NULL;
    InputTouch(tlcdInfo);
    // 보정을 넣은 lcd상의 x , y의 포지션
    xpos = tlcdInfo->a * tlcdInfo->x + tlcdInfo->b * tlcdInfo->y + tlcdInfo->c;
    ypos = tlcdInfo->d * tlcdInfo->x + tlcdInfo->e * tlcdInfo->y + tlcdInfo->f;

    // 터치가 된곳의 위치에 따라달라짐
    if (tlcdInfo->pressure >= 180)
    {
        ret = GetBtn(xpos, ypos);
    }
    else
    {
        ret = -1;
    }
    switch (ret)
    {
    case TOUCH_WHITE:
        shape.outColor = WHITE; //Set OutBound Color WHITE (why!???)
        shape.inColor = WHITE;  //Set InBound Color WHITE
        break;
    case TOUCH_ORANGE:
        shape.outColor = ORANGE; //Set OutBound Color ORANGE
        shape.inColor = ORANGE;  //Set InBound Color ORANGE
        break;
    case TOUCH_RED:
        shape.outColor = RED; //Set OutBound Color RED
        shape.inColor = RED;  //Set InBound Color RED
        break;
    case TOUCH_GREEN:
        shape.outColor = GREEN; //Set OutBound Color GREEN
        shape.inColor = GREEN;  //Set InBound Color GREEN
        break;
    case TOUCH_YELLOW:
        shape.outColor = YELLOW; //Set OutBound Color YELLOW
        shape.inColor = YELLOW;  //Set InBound Color YELLOW
        break;
    case TOUCH_NAVY:
        shape.outColor = NAVY; //Set OutBound Color NAVY
        shape.inColor = NAVY;  //Set InBound Color NAVY
        break;
    case TOUCH_BLUE:
        shape.outColor = BLUE; //Set OutBound Color BLUE
        shape.inColor = BLUE;  //Set InBound Color BLUE
        break;
    case TOUCH_BLACK:
        shape.outColor = BLACK; //Set OutBound Color BLACK
        shape.inColor = BLACK;  //Set InBound Color BLACK
        break;
    case TOUCH_SEL:
        break;
    case TOUCH_ERASE:
        break;
    case TOUCH_CLEAR:
        state = ret;
        g_drawTable[state](tlcdInfo, &shape);
        break;
    case TOUCH_CANVAS:
        if (state >= 0 && state < 4)
        {
            // set Up Start x , y pos
            shape.start.x = xpos;
            shape.start.y = ypos;
            // set Up End x , y pos
            g_drawTable[state](tlcdInfo, &shape);      //함수 불러짐
            struct ListNode *node = CreateNode(shape); //도형에 대한 노드를 만들어서
            Append(node);                              //리스트에 노드 저장
        }
        break;
    default:
        /*
         * 터치를 때면 ret이 -1이 되기 때문에
         * 항상 -1이 들어오는 오류 방지
         */
        if (ret != -1)
        {
            state = ret;
        }
        break;
    }
}
