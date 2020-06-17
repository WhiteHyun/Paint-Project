#include "btn.h"
#include "set.h"

int GetBtn(TLCD tlcdInfo, int xpos, int ypos)
{
    int inputBtnFlag = 0x00000;

    // line flag Up
    if ((xpos >= 270 && xpos <= 320) && (ypos >= 175 && ypos <= 205))
    {
        inputBtnFlag = 1; // 00 0000 0000 0000 0001 Pen
    }
    else if ((xpos >= 270 && xpos <= 320) && (ypos >= 210 && ypos <= 240))
    {
        inputBtnFlag = (1 << 1); // 00 0000 0000 0000 0010 Fill
    }
    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 0 && ypos <= 30))
    {
        inputBtnFlag = (1 << 2); // 00 0000 0000 0000 0100 Line
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 35 && ypos <= 65))
    {
        inputBtnFlag = (1 << 3); // 00 0000 0000 0000 1000 Rectangle
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 70 && ypos <= 100))
    {
        inputBtnFlag = (1 << 4); // 00 0000 0000 0001 0000 Oval
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 105 && ypos <= 135))
    {
        inputBtnFlag = (1 << 5); // 00 0000 0000 0010 0000 Free Draw
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 140 && ypos <= 170))
    {
        inputBtnFlag = (1 << 6); // 00 0000 0000 0100 0000 Select
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 175 && ypos <= 205))
    {
        inputBtnFlag = (1 << 7); // 00 0000 0000 1000 0000 Erase
    }

    else if ((xpos >= 0 && xpos <= 50) && (ypos >= 210 && ypos <= 240))
    {
        inputBtnFlag = (1 << 8); // 00 0000 0001 0000 0000 Clear
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 0 && ypos <= 42))
    {
        inputBtnFlag = (1 << 9); // 00 0000 0010 0000 0000 WhiteColor
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 0 && ypos <= 42))
    {
        inputBtnFlag = (1 << 10); // 00 0000 0100 0000 0000 OrangeColor
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 44 && ypos <= 86))
    {
        inputBtnFlag = (1 << 11); // 00 0000 1000 0000 0000 RedColor
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 44 && ypos <= 86))
    {
        inputBtnFlag = (1 << 12); // 00 0001 0000 0000 0000 GreenColor
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 88 && ypos <= 130))
    {
        inputBtnFlag = (1 << 13); // 00 0010 0000 0000 0000 YellowColor
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 88 && ypos <= 130))
    {
        inputBtnFlag = (1 << 14); // 00 0100 0000 0000 0000 NavyColor
    }

    else if ((xpos >= 272 && xpos <= 295) && (ypos >= 132 && ypos <= 174))
    {
        inputBtnFlag = (1 << 15); // 00 1000 0000 0000 0000 BlueColor
    }

    else if ((xpos >= 297 && xpos <= 320) && (ypos >= 132 && ypos <= 174))
    {
        inputBtnFlag = (1 << 16); // 00 0000 0100 0000 0000 BlackColor
    }

    else if ((xpos >= 60 && xpos <= 260) && (ypos >= 10 && ypos <= 230))
    {
        inputBtnFlag = (1 << 17); // 10 0000 0000 0000 0000 DrawScreen
    }

    return inputBtnFlag;
}

void SensingTouch(TLCD tlcdInfo)
{
    int x, y, pressure;
    int xpos, ypos, ret;

    read(tlcdInfo.fd, &ie, sizeof(struct input_event));

    if (ie.type == 3)
    {
        if (ie.code == 0)
        {
            x = ie.value;
        }
        else if (ie.code == 1)
        {
            y = ie.value;
        }
        else if (ie.code == 24)
        {
            pressure = ie.value;
        }
    }

    // 보정을 넣은 lcd상의 x , y의 포지션
    xpos = a * x + b * y + c;
    ypos = d * x + e * y + f;

    // 터치가 된곳의 위치에 따라달라짐
    if (pressure >= 180)
    {
        ret = GetBtn(tlcdInfo, xpos, ypos);
    }
    else
    {
        ret = 0;
    }

    switch (ret)
    {
    case 1:
        state = 1;
        break;
    case (1 << 1):
        state = 2;
        break;
    case (1 << 2):
        state = 3;
        break;
    case (1 << 3):
        state = 4;
        break;
    case (1 << 4):
        state = 5;
        break;
    case (1 << 5):
        state = 6;
        break;
    case (1 << 6):
        state = 7;
        break;
    case (1 << 7):
        state = 8;
        break;
    case (1 << 8):
        printf("todo Clear\n");
        break;
    case (1 << 9):
        printf("todo WhiteColor\n");
        break;
    case (1 << 10):
        printf("todo OrangeColor\n");
        break;
    case (1 << 11):
        printf("todo RedColor\n");
        break;
    case (1 << 12):
        printf("todo GreenColor\n");
        break;
    case (1 << 13):
        printf("todo YellowColor\n");
        break;
    case (1 << 14):
        printf("todo NavyColor\n");
        break;
    case (1 << 15):
        printf("todo BlueColor\n");
        break;
    case (1 << 16):
        printf("todo BlackColor\n");
        break;
    case (1 << 17):
        printf("touching screen %d %d\n", xpos, ypos);

        switch (state)
        {
        // TODO PEN
        case 1:
            printf("dot screen %d %d\n", xpos, ypos);
            break;
        // TODO Fill
        case 2:
            printf("TODO Fill\n");
            break;
        // TODO LINE
        case 3:
            printf("TODO LINE\n");
            break;
        // TODO RECTANGLE
        case 4:
            printf("TODO RECTANGLE\n");
            break;
        // TODO Oval
        case 5:
            printf("TODO Oval\n");
            break;
        // TODO Oval
        case 6:
            printf("TODO FreeDraw\n");
            break;
        // TODO Select
        case 7:
            printf("TODO Select\n");
            break;
        // TODO Erase
        case 8:
            printf("TODO Erase\n");
            break;
        default:
            break;
        }

        break;
    default:
        break;
    }
}
