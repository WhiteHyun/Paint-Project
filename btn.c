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

int GetBtn(int xpos, int ypos)
{
    int inputBtnFlag = -1;

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
    shape.moveX = 0;
    shape.moveY = 0;
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
        printf("COLOR WHITE\n");
        selectedColor = WHITE;
        break;

    case TOUCH_ORANGE:
        printf("COLOR ORANGE\n");
        selectedColor = ORANGE;
        break;

    case TOUCH_RED:
        printf("COLOR RED\n");
        selectedColor = RED;
        break;

    case TOUCH_GREEN:
        printf("COLOR GREEN\n");
        selectedColor = GREEN;
        break;

    case TOUCH_YELLOW:
        printf("COLOR YELLOW\n");
        selectedColor = YELLOW;
        break;

    case TOUCH_NAVY:
        printf("COLOR NAVY\n");
        selectedColor = NAVY;
        break;

    case TOUCH_BLUE:
        printf("COLOR BLUE\n");
        selectedColor = BLUE;
        break;

    case TOUCH_BLACK:
        printf("COLOR BLACK\n");
        selectedColor = BLACK;
        break;

    case TOUCH_CLEAR: //클리어 모드
        g_drawTable[ret](tlcdInfo, &shape);
        mode = TOUCH_PEN; //화면 클리어 후 펜모드로 바꿈
        break;

    case TOUCH_CANVAS:
        // set Up Start x , y pos
        shape.start.x = xpos;
        shape.start.y = ypos;

        //펜모드이면서 그림유형도 선택되어있는 경우 정상적인 그림 그리기
        if (mode == TOUCH_PEN && state >= 0 && state < 4)
        {
            //Set color
            shape.inColor = WHITE;
            shape.outColor = selectedColor;

            g_drawTable[state](tlcdInfo, &shape);      //함수 불러짐
            struct ListNode *node = CreateNode(shape); //도형에 대한 노드를 만들어서
            Append(node);                              //리스트에 노드 저장
        }

        //설정모드: 채우기 모드거나 선택 모드거나 지우기모드일 때
        else if (mode == TOUCH_FILL || mode == TOUCH_SEL || mode == TOUCH_ERASE)
        {
            //FILL모드일 경우 inColor 속성을 가지고 파라미터로 전달함
            shape.inColor = selectedColor;
            g_drawTable[mode](tlcdInfo, &shape);
        }

        break;

    default:

        /*
         * 쓰레기 값이 들어오는 것을 방지
         */
        if (ret != -1)
        {
            if (ret >= 4 && ret <= 8) //모드 설정
            {
                mode = ret;
            }

            else //그리기 설정
            {
                state = ret;
                mode = TOUCH_PEN; //그리기 설정을 했을 경우 모드는 펜으로 자동설정
            }
        }

        break;
    }
}
