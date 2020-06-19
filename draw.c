#include "draw.h"
#include "ui.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>

inline void InputTouch(TLCD* tlcdInfo)
{
    read(tlcdInfo->fd, &tlcdInfo->ie, sizeof(struct input_event));

    if (tlcdInfo->ie.type == 3)
    {
        if (tlcdInfo->ie.code == 0)
        {
            tlcdInfo->x = tlcdInfo->ie.value;
        }

        else if (tlcdInfo->ie.code == 1)
        {
            tlcdInfo->y = tlcdInfo->ie.value;
        }

        else if (tlcdInfo->ie.code == 24)
        {
            tlcdInfo->pressure = tlcdInfo->ie.value;
        }
    }
}

/*
 * This is Base Code for Making Line Made by D.S Kim
 * Make start x , y -> end x , y Line
 * « ø‰«—±‚¥…  -> ¿‘∑¬πﬁ¿∫ ¡°¿Ã √≥¿Ω ¬Ô»˘ ¡°¿œ∞ÊøÏ start x, yø° ¿˚∏≥
 *             -> æ∆¥“∞ÊøÏ end¿« x,y¡¬«•∏¶ ∞Ëº”«œø© ∞ªΩ≈«œø©¡›¥œ¥Ÿ.
 *             -> ∞ªΩ≈«ﬂ¿ªΩ√ ¿Ã¿¸ø° ±◊∑¡¡¯ Line¿« ¡¬«•∏¶ ∆Ø¡§«œø© ¡ˆøˆ¡›¥œ¥Ÿ (Ω√¿€«“ ∂ß ¡˝æÓ¡÷∏È µ…µÌ?)
 *             -> ( ¿Ã ±‚¥…¿Ã «ŸΩ…¿˚¿” )
 */
void DrawLine(TLCD tlcdInfo, Shape* shape)
{
    int i, tmp, offset;
    int startXpos, startYpos;
    int endXpos, endYpos;
    double incline; //±‚øÔ±‚
    double yIntercept; //y¿˝∆Ì

    while (1) //Ω√¿€¡ˆ¡°¿« x, y¡¬«• ¿‘∑¬
    {
        InputTouch(&tlcdInfo);

        if (tlcdInfo.pressure == 0)
        {
            startXpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
            startYpos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;

            break;
        }
    }

    tlcdInfo.pressure = -1;

    while (1) //¡æ∑·¡ˆ¡°¿« x, y¡¬«• ¿‘∑¬
    {
        InputTouch(&tlcdInfo);

        if (tlcdInfo.pressure == 0)
        {
            endXpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
            endYpos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;

            break;
        }
    }

    if (startXpos < endXpos) //1, 4 ªÁ∫–∏È
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos)); //±‚øÔ±‚ = y¡ı∞°∑Æ / x¡ı∞°∑Æ
        yIntercept = (double)(endYpos - incline * endXpos); //y¿˝∆Ì = y - ±‚øÔ±‚ * x

        for (i = startXpos; i <= endXpos; i++)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo.pfbdata + offset) = 0;
        }
    }

    else //2, 3 ªÁ∫–∏È
    {
        incline = (double)((double)(endYpos - startYpos) / (double)(endXpos - startXpos));
        yIntercept = (double)(endYpos - incline * endXpos);

        for (i = startXpos; i >= endXpos; i--)
        {
            offset = (int)(incline * i + yIntercept) * 320 + (i);
            *(tlcdInfo.pfbdata + offset) = 0;
        }
    }
}

/*
 * This is Base Code for Making Rectangle Made by T.H Kim
 * Make start x , y -> end x , y Rectange
 * « ø‰«—±‚¥…  -> ¿‘∑¬πﬁ¿∫ ¡°¿Ã √≥¿Ω ¬Ô»˘ ¡°¿œ∞ÊøÏ start x, yø° ¿˚∏≥
 *             -> æ∆¥“∞ÊøÏ end¿« x,y¡¬«•∏¶ ∞Ëº”«œø© ∞ªΩ≈«œø©¡›¥œ¥Ÿ.
 *             -> ∞ªΩ≈«ﬂ¿ªΩ√ ¿Ã¿¸ø° ±◊∑¡¡¯ Box∏¶ ¡ˆøˆ¡›¥œ¥Ÿ (Ω√¿€«“ ∂ß ¡˝æÓ¡÷∏È µ…µÌ?)
 *             -> ( ¿Ã ±‚¥…¿Ã «ŸΩ…¿˚¿” )
 */
void DrawRectangle(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawRectangle Executed\n");

    int i, tmp, offset;

    if (shape->start.x > shape->end.x)
    {
        tmp = shape->start.x;
        shape->start.x = shape->end.x;
        shape->end.x = tmp;
    }

    if (shape->start.y > shape->end.y)
    {
        tmp = shape->start.y;
        shape->start.y = shape->end.y;
        shape->end.y = tmp;
    }

    for (i = shape->start.x; i < shape->end.x; i++)
    {
        offset = shape->start.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;

        offset = shape->end.y * 320 + i;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }

    for (i = shape->start.y; i < shape->end.y; i++)
    {
        offset = i * 320 + shape->start.x;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;

        offset = i * 320 + shape->end.x;
        *(tlcdInfo.pfbdata + offset) = shape->outColor;
    }
}

/*
 * This is Base Code for Making Oval Made by D.E Kim
 * Make start x , y -> end x , y Oval
 * « ø‰«—±‚¥…  -> ¿‘∑¬πﬁ¿∫ ¡°¿Ã √≥¿Ω ¬Ô»˘ ¡°¿œ∞ÊøÏ start x, yø° ¿˚∏≥
 *             -> æ∆¥“∞ÊøÏ end¿« x,y¡¬«•∏¶ ∞Ëº”«œø© ∞ªΩ≈«œø©¡›¥œ¥Ÿ.
 *             -> ∞ªΩ≈«ﬂ¿ªΩ√ ¿Ã¿¸ø° ±◊∑¡¡¯ Oval¿ª ¡ˆøˆ¡›¥œ¥Ÿ (Ω√¿€«“ ∂ß ¡˝æÓ¡÷∏È µ…µÌ?)
 *             -> ( ¿Ã ±‚¥…¿Ã «ŸΩ…¿˚¿” )
 */
void DrawOval(TLCD tlcdInfo, Shape* shape)
{
    /* TODO: Draw Oval */
    printf("DrawOval Executed\n");
}

/*
 * This is Base Code for Making DrawFree Made by S.H Hong
 */
void DrawFree(TLCD tlcdInfo, Shape* shape)
{
    /* æ∆¡˜ πÃ ±∏«ˆµ» ªÛ≈¬¿” */
    printf("DrawFree Executed\n");

<<<<<<< HEAD
    struct ListNode *node = NULL;
    int xpos, ypos, i, offset;
    //ÎèÑÌòï ÌÅ¨Í∏∞ ÎèôÏ†Å Ìï†Îãπ
    shape->position = (int **)malloc(sizeof(int *) * SIZEOF_CANVAS_Y); //Ï∫îÎ≤ÑÏä§Ïùò yÌÅ¨Í∏∞: 220
    for (i = 0; i < 220; i++)
    {
        shape->position[i] = (int *)malloc(sizeof(int) * SIZEOF_CANVAS_X); //Ï∫îÎ≤ÑÏä§Ïùò xÌÅ¨Í∏∞: 200
    }

    /* ÌÑ∞Ïπò ÏûÖÎ†•ÏùÑ Î∞õÏùå */
    while (1)
    {
        InputTouch(&tlcdInfo);
        /*ÏΩîÎìú Íµ¨ÌòÑ*/
        xpos = tlcdInfo.a * tlcdInfo.x + tlcdInfo.b * tlcdInfo.y + tlcdInfo.c;
        ypos = tlcdInfo.d * tlcdInfo.x + tlcdInfo.e * tlcdInfo.y + tlcdInfo.f;
        shape->position[ypos - START_CANVAS_Y][xpos - START_CANVAS_X] = 1;
        offset = ypos * 320 + xpos;
        printf("xpos: %d\nypos: %d\n", xpos, ypos);
        *(tlcdInfo.pfbdata + offset) = BLACK;
        offset = shape->end.y * 320 + i;
=======
    struct ListNode* node = NULL;
    int xpos, ypos;

    while (1)
    {
        InputTouch(&tlcdInfo);

>>>>>>> 814c3c0... Make DrawLine
        if (tlcdInfo.pressure == 0)
        {
            //Test code
            printf("xpos = %d\nypos=%d\n", xpos, ypos);
            break;
            /* position ÎèôÏ†Å Ìï†Îãπ */
        }
    }
}

/*
 * TODO
 */
void DrawSelect(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawSelect Executed\n");

    return;
}

/*
 * TODO
 */
void DrawErase(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawErase Executed\n");

    return;
}

/*
 * TODO
 */
void DrawClear(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawClear Executed\n");

    return;
}

/*
 * TODO
 */
void DrawPen(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawPen Executed\n");

    return;
}

/*
 * TODO
 */
void DrawFill(TLCD tlcdInfo, Shape* shape)
{
    printf("DrawFill Executed\n");

    return;
}