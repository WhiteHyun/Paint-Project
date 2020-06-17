#include "set.h"
#include "draw.h"
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

void Run()
{
    Point get, start, end;
    TLCD tlcdInfo;
    Shape shape;

    //TLCD초기화, 오류 발생시 exit
    if (Init_TLCD(&tlcdInfo))
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
}