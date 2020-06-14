#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h> /* for ioctl */
#include <sys/mman.h>  /* for mmap */
#include <sys/types.h>
#include <linux/fb.h> /* for fb_var_screeninfo, FBIOGET_VSCREENINFO */
#include <linux/input.h>

#define FBDEVFILE "/dev/fb2"

float a, b, c, d, e, f, k;
int offset;
struct input_event ie;

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

//도형
typedef struct _Shape
{
    int shape;
    Point start;
    Point end;
    unsigned short color;
} Shape;

typedef unsigned char ubyte;

unsigned short makepixel(ubyte r, ubyte g, ubyte b)
{
    return (unsigned short)(((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3));
}

void setCalibration(TLCD tlcdInfo)
{
    int i, j, tt, pressure;
    int x[3], y[3], xd[3] = {50, 150, 300}, yd[3] = {100, 50, 200}; //점 세 개 미리 지정
    unsigned short red = makepixel(255, 0, 0);
    for (tt = 0; tt < 3; tt++)
    {
        for (i = -5; i < 5; i++)
        {
            offset = (yd[tt] + i) * tlcdInfo.fbvar.xres + xd[tt];
            *(tlcdInfo.pfbdata + offset) = red;
            offset = yd[tt] * tlcdInfo.fbvar.xres + xd[tt] + i;
            *(tlcdInfo.pfbdata + offset) = red;
        }
        while (1)
        {
            read(tlcdInfo.fd, &ie, sizeof(struct input_event));
            if (ie.type == 3)
            {
                if (ie.code == 0)
                    x[tt] = ie.value;
                else if (ie.code == 1)
                    y[tt] = ie.value;
                else if (ie.code == 24)
                    pressure = ie.value;
                if (pressure == 0)
                    break;
            }
        }
        //printf("%d %d\n%d %d\n", xd[tt], yd[tt], x[tt], y[tt]);
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

/* 화면 클리어 */
void clearLcd(TLCD tlcdInfo)
{
    int i, j;
    for (i = 0; i < 240; i++)
    {
        for (j = 0; j < 320; j++)
        {
            offset = i * 320 + j;
            *(tlcdInfo.pfbdata + offset) = 0;
        }
    }
}

void makeLineBox(TLCD tlcdInfo, Shape shape)
{
    int i, j, tmp;
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
    for (j = shape.start.x; j < shape.end.x; j++)
    {
        offset = shape.start.y * 320 + j;
        *(tlcdInfo.pfbdata + offset) = shape.color;
        offset = shape.end.y * 320 + j;
        *(tlcdInfo.pfbdata + offset) = shape.color;
    }
    for (i = shape.start.y; i < shape.end.y; i++)
    {
        offset = i * 320 + shape.start.x;
        *(tlcdInfo.pfbdata + offset) = shape.color;
        offset = i * 320 + shape.end.x;
        *(tlcdInfo.pfbdata + offset) = shape.color;
    }
}

/*
 * TLCD의 정보들을 초기화
 */
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

int main(void)
{
    int pressure = -1;
    Point get, start, end;
    TLCD tlcdInfo;
    Shape shape;
    //TLCD초기화
    Init_TLCD(&tlcdInfo);

    clearLcd(tlcdInfo);
    setCalibration(tlcdInfo);
    clearLcd(tlcdInfo);

    shape.color = makepixel(255, 0, 0);
    while (1)
    {
        read(tlcdInfo.fd, &ie, sizeof(struct input_event));
        if (ie.type == 3)
        {
            if (ie.code == 0)
            {
                get.x = ie.value;
            }
            else if (ie.code == 1)
            {
                get.y = ie.value;
            }
            else if (ie.code == 24)
            {
                pressure = ie.value;
                if (pressure == 0)
                {
                    //printf("start\n");
                    shape.start.x = a * get.x + b * get.y + c;
                    shape.start.y = d * get.x + e * get.y + f;
                    break;
                }
            }
        }
    }
    while (1)
    {
        read(tlcdInfo.fd, &ie, sizeof(struct input_event));
        if (ie.type == 3)
        {
            if (ie.code == 0)
            {
                get.x = ie.value;
            }
            else if (ie.code == 1)
            {
                get.y = ie.value;
            }
            else if (ie.code == 24)
            {
                pressure = ie.value;
                if (pressure == 0)
                {
                    //printf("start\n");
                    shape.end.x = a * get.x + b * get.y + c;
                    shape.end.y = d * get.x + e * get.y + f;
                    break;
                }
            }
        }
    }

    makeLineBox(tlcdInfo, shape);

    close(tlcdInfo.fd);
    close(tlcdInfo.fbfd);
    return 0;

} // end of main
