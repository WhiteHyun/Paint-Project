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
 *              0 -> Line / 1 -> Rectangle / 2 -> Oval / 3 -> FreeDraw
 */
static int state = 3;

/*
 * 4 -> Select / 5 -> Erase / 6 -> Clear / 7 -> Pen / 8 -> Fill
 */
static int mode = 7;

//Selected Color
static unsigned short selectedColor = 0;

//Detected when lcd touched, and then set tlcd's x, y info.
inline void InputTouch(TLCD *tlcdInfo);

/*
 * inputBtnFlag 0 -> Line / 1 -> Rectangle / 2 -> Oval 3 -> FreeDraw / 4 -> Select /
 *              5 -> Erase / 6 -> Clear / 7 -> pen / 8 -> Fill / 
 *              9 ~ 16 -> each color / 17 -> touch to drawing paper
 */
int GetBtn(int xpos, int ypos);

// TouchPos Get
void SensingTouch(TLCD *tlcdInfo);

#endif
