#ifndef LIST_H
#define LIST_H

#include "draw.h"

struct ListNode
{
    struct ListNode *next; //다음 노드를 가리키는 포인터
    struct ListNode *prev; //이전 노드를 가리키는 포인터
    Shape shape;           //도형의 정보(data)
};

struct List
{
    struct ListNode *peek;  //리스트의 가장 끝
    struct ListNode *start; //리스트의 시작
    int size;               //리스트의 크기
};

struct List *g_List;

/*
 * 리스트 노드를 할당해주는 함수입니다.
 */
struct ListNode *CreateNode(Shape shape);

/*
 * 리스트 노드를 삭제합니다. 메모리 해제해줍니다.
 */
void DeleteNode(struct ListNode *removeNode);

/*
 * 리스트의 초기 정보를 설정해주는 함수입니다. 초반에 꼭 불러야합니다.
 */
void InitList();

/*
 * Push와 같다고 보시면 됩니다.
 */
void Append(struct ListNode *newNode);

/*
 * 노드의 정보를 반환합니다. 반환된 노드 안에 도형 구조체가 있으니 이를 사용하면 됩니다. 
 */
struct ListNode *Pop();

/*
 * Pop과 같습니다. 노드의 정보를 반환합니다. 반환된 노드 안에 도형 구조체가 있으니 이를 사용하면 됩니다. 
 * 
 * 해당 파라미터를 인덱스로 하여 노드를 반환합니다.
 */
struct ListNode *IndexPop(int index);

/*
 * 두 점에 해당하는 노드가 있는지를 확인하고 있다면 리턴합니다.
 * 만약 아무 도형도 없는 경우 NULL을 리턴합니다.
 */
struct ListNode *SearchShape(int touchX, int touchY);

/*
 * 리스트 내부의 노드(도형)들을 전부 없앱니다.
 */
void ListClear();

#endif
