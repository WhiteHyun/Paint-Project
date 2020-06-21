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
    float a, b, c, d, e, f, k; //터치 하는 위치를 계산하는 값
    struct input_event ie;     //터치 이벤트
    int x, y, pressure;        //이벤트에 따라 발생하는 x,y값과 압력값
} TLCD;

/* 화면 클리어 */
void ClearLcd(TLCD tlcdInfo);

// TLCD의 정보들을 초기화
int Init_TLCD(TLCD *tlcdInfo);

void SetCalibration(TLCD *tlcdInfo);

void Run();

#endif
