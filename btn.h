#ifndef BTN_H
#define BTN_H
#include "draw.h"
#include "set.h"
enum button_touch_state
{
    TOUCH_LINE = 1, //1
    TOUCH_RECT,     //2
    TOUCH_OVAL,     //3
    TOUCH_FREEDRAW, //4
    TOUCH_SEL,      //5
    TOUCH_ERASE,    //6
    TOUCH_CLEAR,    //7
    TOUCH_PEN,      //8
    TOUCH_FILL,     //9
    TOUCH_WHITE,    //10
    TOUCH_ORANGE,   //11
    TOUCH_RED,      //12
    TOUCH_GREEN,    //13
    TOUCH_YELLOW,   //14
    TOUCH_NAVY,     //15
    TOUCH_BLUE,     //16
    TOUCH_BLACK,    //17
    TOUCH_CANVAS    //18
};

/*
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase 
 */
int state;

/*
 * inputBtnFlag 0 -> touch to drawing paper /
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase / 9 -> Clear 
 *              10 ~ 17 -> each color , 18 -> 중앙흰색
 */
int GetBtn(TLCD tlcdInfo, int xpos, int ypos);

// TouchPos Get
void SensingTouch(TLCD tlcdInfo);
#endif