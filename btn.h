#ifndef BTN_H
#define BTN_H
#include "draw.h"
#include "set.h"
enum button_touch_state
{
    TOUCH_LINE,     // 0
    TOUCH_RECT,     // 1
    TOUCH_OVAL,     // 2
    TOUCH_FREEDRAW, // 3
    TOUCH_SEL,      // 4
    TOUCH_ERASE,    // 5
    TOUCH_CLEAR,    // 6
    TOUCH_PEN,      // 7
    TOUCH_FILL,     // 8
    TOUCH_WHITE,    // 9
    TOUCH_ORANGE,   //10
    TOUCH_RED,      //11
    TOUCH_GREEN,    //12
    TOUCH_YELLOW,   //13
    TOUCH_NAVY,     //14
    TOUCH_BLUE,     //15
    TOUCH_BLACK,    //16
    TOUCH_CANVAS    //17
};

/*
 *              1 -> Line / 2 -> Rectangle / 3 -> Oval / 4 -> FreeDraw / 5 -> Select 
 *              6 -> Erase / 7 -> Clear / 8 -> Pen / 9 -> Fill
 */
int state;

/* if isFirst is 1 -> first touch event , if isFirst is -1 -> it's not first touch
 */
int isFirst;

inline void InputTouch(TLCD *tlcdInfo);

/*
 * inputBtnFlag 0 -> touch to drawing paper /
 *              1 -> pen / 2 -> Fill / 3 -> Line / 4 -> Rectangle / 5 -> Oval 
 *              6 -> FreeDraw / 7 -> Select / 8 -> Erase / 9 -> Clear 
 *              10 ~ 17 -> each color , 18 -> 중앙흰색
 */
int GetBtn(int xpos, int ypos);

// TouchPos Get
void SensingTouch(TLCD *tlcdInfo);
#endif