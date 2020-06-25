#include "list.h"
#include "btn.h"
#include "ui.h"
#include <stdlib.h>

struct ListNode *CreateNode(Shape shape)
{
    struct ListNode *node = (struct ListNode *)malloc(sizeof(struct ListNode));

    //성공적으로 할당 된 경우 초기값으로 초기화해줍니다.
    if (node != NULL)
    {
        node->next = NULL;
        node->prev = NULL;
        node->shape = shape;
    }

    return node;
}

void DeleteNode(struct ListNode *removeNode)
{
    int i;

    //노드에 메모리가 할당되어있는 경우 해제해줍니다.
    if (removeNode != NULL)
    {
        //만약 자유그리기 도형이면 메모리 할당을 해제해주는 절차를 걸칩니다.
        if (removeNode->shape.type == TOUCH_FREEDRAW)
        {
            for (i = 0; i < 220; i++)
            {
                free(removeNode->shape.position[i]);
            }

            free(removeNode->shape.position);
        }

        free(removeNode);
    }
}

void InitList()
{
    if (g_List == NULL)
    {
        g_List = (struct List *)malloc(sizeof(struct List));

        if (g_List != NULL)
        {
            g_List->peek = NULL;
            g_List->start = NULL;
            g_List->size = 0;
        }
    }
}

void Append(struct ListNode *newNode)
{
    struct ListNode *tempNode;
    /*
     * 리스트의 사이즈에 따라 넣는 방식이 달라지므로 나눕니다.
     */

    if (g_List->size == 0)
    {
        g_List->peek = newNode;
        g_List->start = newNode;
        g_List->size++;
    }

    else if (g_List->size > 0)
    {
        tempNode = g_List->peek;
        tempNode->next = newNode;
        newNode->prev = tempNode;
        g_List->peek = newNode;
        g_List->size++;
    }
}

struct ListNode *Pop()
{
    struct ListNode *tempNode = NULL;

    //when List is Empty
    if (g_List->size == 0)
    {
        printf("Pop(), Stack is Empty\n");
    }

    //when List is not empty
    else
    {
        tempNode = g_List->peek;
        g_List->peek = g_List->peek->prev;
        g_List->size--;
    }

    return tempNode;
}

struct ListNode *IndexPop(int index)
{
    struct ListNode *tempNode = NULL;

    //when List is Empty
    if (g_List->size == 0)
    {
        printf("Error, Pop(%d), Stack is Empty\n", index);
    }

    //when List is not empty
    else
    {
        //out of index 에러 사전방지
        if (index >= g_List->size)
        {
            printf("Error, IndexPop(%d), Out of Index.\n", index);
        }

        //리스트에 하나의 노드밖에 없거나 맨 뒤의 값을 꺼내오는 경우 일반 Pop 함수를 불러 처리합니다
        else if ((index == 0 && g_List->size == 1) || index == g_List->size - 1)
        {
            return Pop();
        }

        //중간 노드를 팝하는 경우
        else
        {
            tempNode = g_List->start;

            //맨 앞의 값을 Pop하고 싶은 경우 다음 값이 리스트의 첫번째가 됩니다.
            if (index == 0)
            {
                g_List->start = tempNode->next;
            }

            //맨 앞의 값을 빼내는 것이 아니므로 NULL참조가 되지 않습니다. 실행해줘도 됩니다.
            else
            {
                //index에 해당하는 노드까지 이동
                while (index != 0)
                {
                    tempNode = tempNode->next;
                    index--;
                }
                tempNode->prev->next = tempNode->next;
            }

            tempNode->next->prev = tempNode->prev;
            g_List->size--;
        }
    }

    return tempNode;
}

struct ListNode *SearchShape(int touchX, int touchY)
{
    /*
     * 선택된 도형의 노드인덱스를 구하기 위해 index 선언
     * -1한 이유는 인덱스는 0~ size-1까지 있기 때문
     */
    int index = g_List->size - 1;
    struct ListNode *tempNode = g_List->peek;

    //도형이 발견될 때 까지 노드들을 전부 방문
    while (tempNode != NULL)
    {
        //만약 해당 위치에 도형이 있다면
        if (tempNode->shape.type != TOUCH_FREEDRAW)
        {

            if (tempNode->shape.start.x + tempNode->shape.moveX <= touchX && tempNode->shape.start.y + tempNode->shape.moveY <= touchY &&
                tempNode->shape.end.x + tempNode->shape.moveX >= touchX && tempNode->shape.end.y + tempNode->shape.moveY >= touchY)
            {
                break;
            }
        }
        else
        {
            if (tempNode->shape.position[touchY - tempNode->shape.moveY][touchX - tempNode->shape.moveX] == 1)
            {
                break;
            }
        }

        tempNode = tempNode->prev; //도형을 찾지 못했다면 이전 노드로 이동
        --index;                   //인덱스도 감소
    }

    //만약 도형을 찾았다면
    if (tempNode != NULL)
    {
        Append(IndexPop(index)); //선택된 도형을 가장 끝노드로 옮깁니다.
    }
    return tempNode;
}

void ListClear()
{
    int count = g_List->size;
    int i;

    struct ListNode *tempNode;

    for (i = 0; i < count; i++)
    {
        tempNode = Pop();
        DeleteNode(tempNode);
    }

    g_List->peek = NULL;
    g_List->start = NULL;
    g_List->size = 0;
}
