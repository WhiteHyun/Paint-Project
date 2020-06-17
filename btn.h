#ifndef BTN_H
#define BTN_H
#include "draw.h"
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