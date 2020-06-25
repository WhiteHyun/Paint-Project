#include "set.h"
#include "draw.h"
#include "btn.h"
#include "ui.h"
#include "list.h"

extern struct List *g_List;
extern struct Pixel sketchBook[SIZEOF_CANVAS_Y][SIZEOF_CANVAS_X];
void ClearLcd(TLCD tlcdInfo)
{
    int i, j, offset;

    for (i = 0; i < 240; i++)
    {
        for (j = 0; j < 320; j++)
        {
            offset = i * 320 + j;
            *(tlcdInfo.pfbdata + offset) = CYAN;
        }
    }
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

void SetCalibration(TLCD *tlcdInfo)
{
    int i, j, pressure, offset;
    int x[3], y[3], xd[3] = {50, 150, 300}, yd[3] = {100, 50, 200}; //점 세 개 미리 지정
    unsigned short color = RED;                                     //빨간색 지정

    for (j = 0; j < 3; j++)
    {
        for (i = -5; i < 5; i++)
        {
            offset = (yd[j] + i) * tlcdInfo->fbvar.xres + xd[j];
            *(tlcdInfo->pfbdata + offset) = color;

            offset = yd[j] * tlcdInfo->fbvar.xres + xd[j] + i;
            *(tlcdInfo->pfbdata + offset) = color;
        }

        while (1)
        {
            read(tlcdInfo->fd, &tlcdInfo->ie, sizeof(struct input_event));

            if (tlcdInfo->ie.type == 3)
            {
                if (tlcdInfo->ie.code == 0)
                    x[j] = tlcdInfo->ie.value;

                else if (tlcdInfo->ie.code == 1)
                    y[j] = tlcdInfo->ie.value;

                else if (tlcdInfo->ie.code == 24)
                    pressure = tlcdInfo->ie.value;

                if (pressure == 0)
                    break;
            }
        }

        pressure = -1;
    }

    tlcdInfo->k = ((x[0] - x[2]) * (y[1] - y[2])) - ((x[1] - x[2]) * (y[0] - y[2]));
    tlcdInfo->a = ((xd[0] - xd[2]) * (y[1] - y[2])) - ((xd[1] - xd[2]) * (y[0] - y[2]));
    tlcdInfo->b = ((x[0] - x[2]) * (xd[1] - xd[2])) - ((xd[0] - xd[2]) * (x[1] - x[2]));
    tlcdInfo->c = (y[0] * ((x[2] * xd[1]) - (x[1] * xd[2]))) + (y[1] * ((x[0] * xd[2]) - (x[2] * xd[0]))) + (y[2] * ((x[1] * xd[0]) - (x[0] * xd[1])));
    tlcdInfo->d = ((yd[0] - yd[2]) * (y[1] - y[2])) - ((yd[1] - yd[2]) * (y[0] - y[2]));
    tlcdInfo->e = ((x[0] - x[2]) * (yd[1] - yd[2])) - ((yd[0] - yd[2]) * (x[1] - x[2]));
    tlcdInfo->f = (y[0] * ((x[2] * yd[1]) - (x[1] * yd[2]))) + (y[1] * ((x[0] * yd[2]) - (x[2] * yd[0]))) + (y[2] * ((x[1] * yd[0]) - (x[0] * yd[1])));

    tlcdInfo->a = tlcdInfo->a / tlcdInfo->k;
    tlcdInfo->b = tlcdInfo->b / tlcdInfo->k;
    tlcdInfo->c = tlcdInfo->c / tlcdInfo->k;
    tlcdInfo->d = tlcdInfo->d / tlcdInfo->k;
    tlcdInfo->e = tlcdInfo->e / tlcdInfo->k;
    tlcdInfo->f = tlcdInfo->f / tlcdInfo->k;
}
// ErrHandle For Fill
void InitScreen()
{
    int i, j;
    for (i = START_CANVAS_Y; i < END_CANVAS_Y; i++)
    {
        for (j = START_CANVAS_X; j < END_CANVAS_X; j++)
        {
            sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].number = 0;
            sketchBook[i - START_CANVAS_Y][j - START_CANVAS_X].color = WHITE;
        }
    }
}
void Run()
{
    TLCD tlcdInfo;
    //TODO: Create LIST SHAPE

    //TLCD초기화, 오류 발생시 exit
    if (Init_TLCD(&tlcdInfo))
    {
        exit(1);
    }

    ClearLcd(tlcdInfo);
    SetCalibration(&tlcdInfo);
    ClearLcd(tlcdInfo);
    InitList();
    DrawUI(tlcdInfo);
    InitScreen();
    // main code part
    for (;;)
    {
        SensingTouch(&tlcdInfo);
    }

    close(tlcdInfo.fd);
    close(tlcdInfo.fbfd);
}
