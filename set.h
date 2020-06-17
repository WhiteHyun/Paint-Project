#ifndef SET_H
#define SET_H
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
struct input_event ie;
int offset;

/*
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase 
 */
int state;

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

/* 화면 클리어 */
void ClearLcd(TLCD tlcdInfo);

// TLCD의 정보들을 초기화
int Init_TLCD(TLCD *tlcdInfo);

void SetCalibration(TLCD tlcdInfo);

void Run();
#endif