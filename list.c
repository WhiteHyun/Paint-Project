#include "list.h"
#include <stdio.h>
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
    //노드에 메모리가 할당되어있는 경우 해제해줍니다.
    if (removeNode != NULL)
    {
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
    //when stack is not empty
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
        printf("Pop(%d), Stack is Empty\n", index);
    }
    //when stack is not empty
    else
    {
        //out of index 에러 사전방지
        if (index >= g_List->size)
        {
            printf("IndexPop(%d), Out of Index.\n", index);
        }

        //리스트의 하나의 노드밖에 없거나 맨 뒤의 값을 꺼내오는 경우 일반 Pop 함수를 불러 처리합니다
        else if ((index == 0 && g_List->size == 1) || index == g_List->size - 1)
        {
            return Pop();
        }
        else
        {
            tempNode = g_List->start;
            //index에 해당하는 노드까지 이동
            while (index != 0)
            {
                tempNode = tempNode->next;
                index--;
            }

            //맨 앞의 값을 Pop하고 싶은 경우 다음 값이 리스트의 첫번째가 됩니다.
            if (index == 0)
            {
                g_List->start = tempNode->next;
            }

            //맨 앞의 값을 빼내는 것이 아니므로 NULL참조가 되지 않습니다. 실행해줘도 됩니다.
            else
            {
                tempNode->prev->next = tempNode->next;
            }

            tempNode->next->prev = tempNode->prev;
            g_List->size--;
        }
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