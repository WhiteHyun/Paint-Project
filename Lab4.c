#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h>  /* for mmap */
#include <sys/types.h>
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <linux/input.h>
#include "ui.h"

#define FBDEVFILE "/dev/fb2"

#define MAXSHAPENUM 50

/* 
 * TLCD infomation struct.
 * LCD에 출력될시 필요한 정보들을 가지고있는 구조체
 */
typedef struct _TLCD
{
    struct fb_var_screeninfo fbvar; //LCD정보들을 가지고 있음
    int fbfd;                       //파일의 정보를 읽어온 값을 저장할 변수
    unsigned short *pfbdata;        //LCD의 시작주소를 가리킴
    int fd;
} TLCD;

typedef struct _Point
{
    int x;
    int y;
} Point;

/* 
 * Shape infomation struct.
 * shape -> 0 is Pen , 1 is Line , 2 Rectangle , 3 Oval , 4 FreeDraw
 * Point start & end have each x , y pos
 * inColor is inBound Color
 * outColor is outBound Color
 */
typedef struct _Shape
{
    int shape;
    Point start;
    Point end;
    unsigned short inColor;  // inBound Color
    unsigned short outColor; // outBound Color
    // int matrix
} Shape;

typedef unsigned char ubyte;

unsigned short MakePixel(ubyte r, ubyte g, ubyte b)
{
    return (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

float a, b, c, d, e, f, k;
struct input_event ie;
int offset;

/*
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase 
 */
int state;

/* 화면 클리어 */
void ClearLcd(TLCD tlcdInfo);

/*
 * inputBtnFlag 0 -> touch to drawing paper /
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase / 9 -> Clear 
 *              10 ~ 17 -> each color , 18 -> 중앙흰색
 */
int GetBtn(TLCD tlcdInfo, int xpos, int ypos);

// TLCD의 정보들을 초기화
int Init_TLCD(TLCD *tlcdInfo);

//기본 UI 출력
void DrawUI(TLCD tlcdInfo);

// TouchPos Get
void SensingTouch(TLCD tlcdInfo);

// make Rectangle Base Code
void MakeLineBox(TLCD tlcdInfo, Shape shape);

void SetCalibration(TLCD tlcdInfo);

int main(void)
{
    Point get, start, end;
    TLCD tlcdInfo;
    Shape shape;

    //TLCD초기화, 오류 발생시 exit
    if (!Init_TLCD(&tlcdInfo))
    {
        exit(1);
    }

    ClearLcd(tlcdInfo);
    SetCalibration(tlcdInfo);
    ClearLcd(tlcdInfo);

    DrawUI(tlcdInfo);

    // main code part
    for (;;)
    {
        SensingTouch(tlcdInfo);
    }

    close(tlcdInfo.fd);
    close(tlcdInfo.fbfd);
    return 0;

} // end of main

void ClearLcd(TLCD tlcdInfo)
{
    int i, j;
    for (i = 0; i < 240; i++)
    {
        for (j = 0; j < 320; j++)
        {
            offset = i * 320 + j;
            *(tlcdInfo.pfbdata + offset) = MakePixel(0, 204, 135);
        }
    }
}

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

int Init_TLCD(TLCD *tlcdInfo)
{
    int ret = -1;

    //frame buffer의 node인 /dev/fb를 open
    if ((tlcdInfo->fbfd = open(FBDEVFILE, O_RDWR)) < 0)
    {
        perror("fbdev open");
        goto done;
    }

    //FBIOGET_VSCREENINFO: Kernel로 부터 fb_var_screeninfo 정보를 가져올 때 쓰는 옵션, fbvar에 저장한다.
    ret = ioctl(tlcdInfo->fbfd, FBIOGET_VSCREENINFO, &(tlcdInfo->fbvar));

    if (ret < 0) //정보 읽기에 실패한 경우
    {
        perror("fbdev ioctl");
        goto done;
    }

    if (tlcdInfo->fbvar.bits_per_pixel != 16)
    {
        fprintf(stderr, "bpp is not 16\n");
        goto done;
    }

    //mmap 사용
    tlcdInfo->pfbdata = (unsigned short *)mmap(0, tlcdInfo->fbvar.xres * tlcdInfo->fbvar.yres * 16 / 8, PROT_READ | PROT_WRITE, MAP_SHARED, tlcdInfo->fbfd, 0);

    if ((unsigned)tlcdInfo->pfbdata == (unsigned)-1)
    {
        perror("fbdev mmap");
        goto done;
    }

    //이벤트 추가
    if ((tlcdInfo->fd = open("/dev/input/event4", O_RDONLY)) < 0)
    {
        printf("fd open error!\n");
        goto done;
    }

    ret = 0;

done:
    return ret;
}

void DrawUI(TLCD tlcdInfo)
{
    int i, j, k;
    int rectSpace = 0;     //버튼 사이의 간격
    int startSpace = 0;    //TFT_LCD의 왼쪽 면과 버튼의 간격, 0으로 초기화했지만, 혹시 모르기때문에 사용
    int buttonHeight = 30; //버튼의 높이
    int buttonWidth = 50;  //버튼의 폭
    int buttonCount = 0;   //글자 배열내 요소들을 하나씩 카운트함
    int *buttonAlphabet;   //포인터 배열, 버튼들을 하나씩 받아와 출력
    int color = 0;         //좌측 8개의 색을 저장할 변수

    for (k = 0; k < 7; k++) //좌측 7개의 버튼, 흰색 칸 생성
    {
        for (i = 0; i < buttonHeight; i++) //각 버튼의 높이
        {
            for (j = 0; j < buttonWidth; j++) //각 버튼의 폭
            {
                offset = (i + rectSpace) * 320 + (j + startSpace);
                *(tlcdInfo.pfbdata + offset) = 65535; //흰색
            }
        }

        rectSpace += 35; //버튼 사이의 간격 5
    }                    //30 * 7 + 5 * 6 = 240, 딱맞음

    rectSpace = 0;  //처음부터 출력해야하므로 초기화
    startSpace = 0; //좌측 7개 버튼의 범위 - 폭 0 ~ 50, 높이 0 ~ 240

    for (k = 0; k < 7; k++) //좌측 7개 버튼의 글자 출력
    {
        if (k == 0)
        {
            buttonAlphabet = LINE;
        }

        else if (k == 1)
        {
            buttonAlphabet = RECT;
        }

        else if (k == 2)
        {
            buttonAlphabet = OVAL;
        }

        else if (k == 3)
        {
            buttonAlphabet = FREEDRAW;
        }

        else if (k == 4)
        {
            buttonAlphabet = SELECT;
        }

        else if (k == 5)
        {
            buttonAlphabet = ERASE;
        }

        else
        {
            buttonAlphabet = CLEAR;
        }

        for (i = 0, buttonCount = 0; buttonCount < 1500; i++, buttonCount++) //글자배열의 크기 50 * 30 = 1500
        {
            if (buttonAlphabet[buttonCount] == 0) //0이면 출력할 필요가 없으니 생략
                continue;

            else
            {
                if (i >= buttonWidth) //i가 버튼의 높이보다 커진다면
                {
                    i %= buttonWidth; //줄바꿈
                }

                j = buttonCount / buttonWidth; //j는 버튼의 폭으로 계속 나누어서 0 ~ buttonWidth까지 반복

                offset = (j + rectSpace) * 320 + (i + startSpace);
                *(tlcdInfo.pfbdata + offset) = 0;
            }
        }

        rectSpace += 35; //버튼 사이의 간격 5
    }

    rectSpace = 240 - buttonHeight * 2 - 5; //우측 버튼은 두개이므로, 버튼 높이의 두배를 빼주고, 버튼 사이의 간격이 5px이므로 5를 한번 더 빼줌
    startSpace = 320 - buttonWidth;         //우측 2개 버튼의 범위 - 폭 270 ~ 320, 높이 175 ~ 240

    for (k = 0; k < 2; k++) //우측 2개의 버튼, 흰색 칸 생성
    {
        for (i = 0; i < buttonHeight; i++) //각 버튼의 높이
        {
            for (j = 0; j < buttonWidth; j++) //각 버튼의 폭
            {
                offset = (i + rectSpace) * 320 + (j + startSpace);
                *(tlcdInfo.pfbdata + offset) = 65535; //흰색
            }
        }

        rectSpace += 35; //버튼 사이의 간격 5
    }

    rectSpace = 240 - buttonHeight * 2 - 5; //우측 버튼은 두개이므로, 버튼 높이의 두배를 빼주고, 버튼 사이의 간격이 5px이므로 5를 한번 더 빼줌
    startSpace = 320 - buttonWidth;         //320 - buttonWidth

    for (k = 0; k < 2; k++) //우측 2개 버튼의 글자 출력
    {
        if (k == 0)
        {
            buttonAlphabet = PEN;
        }

        else
        {
            buttonAlphabet = FILL;
        }

        for (i = 0, buttonCount = 0; buttonCount < 1500; i++, buttonCount++) //글자배열의 크기 50 * 30 = 1500
        {
            if (buttonAlphabet[buttonCount] == 0) //0이면 출력할 필요가 없으니 생략
                continue;

            else
            {
                if (i >= buttonWidth) //i가 버튼의 높이보다 커진다면
                {
                    i %= buttonWidth; //줄바꿈
                }

                j = buttonCount / buttonWidth; //j는 버튼의 폭으로 계속 나누어서 0 ~ buttonWidth까지 반복

                offset = (j + rectSpace) * 320 + (i + startSpace);
                *(tlcdInfo.pfbdata + offset) = 0;
            }
        }

        rectSpace += 35; //버튼 사이의 간격 5
    }

    rectSpace = 0;
    startSpace = 320 - buttonWidth; //우측 8개 버튼의 범위 - 폭 272 ~ 320, 높이 0 ~ 174

    for (k = 0; k < 8; k++) //우측 8개의 색 출력
    {
        if (k == 0)
        {
            color = 65535; //흰색
        }

        else if (k == 1)
        {
            color = MakePixel(255, 192, 0);
        }

        else if (k == 2)
        {
            color = MakePixel(255, 0, 0);
        }

        else if (k == 3)
        {
            color = MakePixel(146, 208, 80);
        }

        else if (k == 4)
        {
            color = MakePixel(255, 255, 0);
        }

        else if (k == 5)
        {
            color = MakePixel(0, 32, 96);
        }

        else if (k == 6)
        {
            color = MakePixel(0, 112, 192);
        }

        else
        {
            color = 0;
        }

        for (i = 0; i < 42; i++) //색 버튼 한개의 높이 42
        {
            for (j = 0; j < 23; j++) //색 버튼 한개의 폭 23
            {
                offset = (i + rectSpace) * 320 + (j + startSpace);
                *(tlcdInfo.pfbdata + offset) = color;
            }
        }

        if (k % 2 == 1)
        {
            rectSpace += 44; //42 + 2
            startSpace -= 25;
        }

        else
        {
            startSpace += 25; //23 + 2 = 23
        }
    }

    startSpace = 60; //가운데 흰 사각형 출력
    rectSpace = 10;  //흰 사각형의 범위 - 폭 60 ~ 260, 높이 10 ~ 230

    for (i = 0; i < 220; i++) //색 버튼 한개의 높이 42
    {
        for (j = 0; j < 200; j++) //색 버튼 한개의 폭 23
        {
            offset = (i + rectSpace) * 320 + (j + startSpace);
            *(tlcdInfo.pfbdata + offset) = 65535;
        }
    }
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

void MakeLineBox(TLCD tlcdInfo, Shape shape)
{
    int i, tmp;

    if (shape.start.x > shape.end.x)
    {
        tmp = shape.start.x;
        shape.start.x = shape.end.x;
        shape.end.x = tmp;
    }

    if (shape.start.y > shape.end.y)
    {
        tmp = shape.start.y;
        shape.start.y = shape.end.y;
        shape.end.y = tmp;
    }

    for (i = shape.start.x; i < shape.end.x; i++)
    {
        offset = shape.start.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
        offset = shape.end.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
    }

    for (i = shape.start.y; i < shape.end.y; i++)
    {
        offset = i * 320 + shape.start.x;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
        offset = i * 320 + shape.end.x;
        *(tlcdInfo.pfbdata + offset) = shape.outColor;
    }
}

void SetCalibration(TLCD tlcdInfo)
{
    int i, j, pressure;
    int x[3], y[3], xd[3] = {50, 150, 300}, yd[3] = {100, 50, 200}; //점 세 개 미리 지정
    unsigned short red = MakePixel(255, 0, 0);

    for (j = 0; j < 3; j++)
    {
        for (i = -5; i < 5; i++)
        {
            offset = (yd[j] + i) * tlcdInfo.fbvar.xres + xd[j];
            *(tlcdInfo.pfbdata + offset) = red;

            offset = yd[j] * tlcdInfo.fbvar.xres + xd[j] + i;
            *(tlcdInfo.pfbdata + offset) = red;
        }
        while (1)
        {
            read(tlcdInfo.fd, &ie, sizeof(struct input_event));

            if (ie.type == 3)
            {
                if (ie.code == 0)
                    x[j] = ie.value;

                else if (ie.code == 1)
                    y[j] = ie.value;

                else if (ie.code == 24)
                    pressure = ie.value;

                if (pressure == 0)
                    break;
            }
        }

        //printf("%d %d\n%d %d\n", xd[j], yd[j], x[j], y[j]);
        pressure = -1;
    }

    k = ((x[0] - x[2]) * (y[1] - y[2])) - ((x[1] - x[2]) * (y[0] - y[2]));
    a = ((xd[0] - xd[2]) * (y[1] - y[2])) - ((xd[1] - xd[2]) * (y[0] - y[2]));
    b = ((x[0] - x[2]) * (xd[1] - xd[2])) - ((xd[0] - xd[2]) * (x[1] - x[2]));
    c = (y[0] * ((x[2] * xd[1]) - (x[1] * xd[2]))) + (y[1] * ((x[0] * xd[2]) - (x[2] * xd[0]))) + (y[2] * ((x[1] * xd[0]) - (x[0] * xd[1])));
    d = ((yd[0] - yd[2]) * (y[1] - y[2])) - ((yd[1] - yd[2]) * (y[0] - y[2]));
    e = ((x[0] - x[2]) * (yd[1] - yd[2])) - ((yd[0] - yd[2]) * (x[1] - x[2]));
    f = (y[0] * ((x[2] * yd[1]) - (x[1] * yd[2]))) + (y[1] * ((x[0] * yd[2]) - (x[2] * yd[0]))) + (y[2] * ((x[1] * yd[0]) - (x[0] * yd[1])));

    a = a / k;
    b = b / k;
    c = c / k;
    d = d / k;
    e = e / k;
    f = f / k;
}
