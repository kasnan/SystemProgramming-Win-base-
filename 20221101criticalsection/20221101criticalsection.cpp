﻿// 20211110criticalsectionObject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
// 20211110criticalsection.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
// 20211103linkedlist.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

/*
1. Linked list 구조체 정의
2. 모든 함수 재정의
3. race condition 데모
4. critical setion을 사용한 동기화
 */

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

 //구조체 Node
typedef struct node {
    int data;
    struct node* pNext;
} Node;
//구조체 List
typedef struct LinkedList {
    Node* pHead;
    CRITICAL_SECTION criticalSection;//깃발 추가
} List;
//함수: createList()
//입력: 없음
//출력: List 구조체의 객체를 생성하여 이 포인터를 반환
LinkedList* createList() {
    LinkedList* pList = (LinkedList*)malloc(sizeof(LinkedList)); //list 초기화
    pList->pHead = NULL;
    InitializeCriticalSection(&(pList->criticalSection)); //Critical Section 초기화
    return pList;
}
//함수: deleteList()
// 입력: 리스트 포인터
// 출력: 없음
void deleteList(LinkedList* pList) {
    DeleteCriticalSection(&(pList->criticalSection));
    free(pList);
}
//함수: createNode()
//입력: data 값
//출력: 생성한 노드의 포인터, 
//노드의 데이터값은 입력값, pNext = NULL
Node* createNode(int value) {
    Node* ptr;
    ptr = (Node*)malloc(sizeof(Node));
    ptr->data = value;
    ptr->pNext = NULL;
    return ptr;
}
void printLL(LinkedList* pList) {
    Node* ptr = pList->pHead;
    while (ptr != NULL) {
        printf("data = %d\n", ptr->data);
        ptr = ptr->pNext;
    }
}
//함수: countNode()
//입력: pHead
//출력: 노드 갯수
int countNode(LinkedList* pList) {
    Node* ptr = pList->pHead;
    int count = 0;
    while (ptr != NULL) {
        count++;
        ptr = ptr->pNext;
    }
    return count;
}
//함수: insertHead()
//입력: 헤드노드포인터, 새 노드 포인터
//출력: 없음
void insertHead(LinkedList* pList, Node* newNode) { //깃발올라가 있는 상태 : Signaled State, 깃발 내려가있는 상태 : Unsignaled State
    EnterCriticalSection(&(pList->criticalSection));//깃발확인, 만약 깃발이 올라가 있으면 다음 문장으로 가고 깃발을 내린다. 깃발이 내려가 있으면 올라갈때까지 대기한다.
    newNode->pNext = pList->pHead; //#1
    pList->pHead = newNode; //#2
    LeaveCriticalSection(&(pList->criticalSection));//깃발을 올린다.
}

DWORD WINAPI ThreadFunc(LPVOID);// LPVOID ==> void*

int main()
{
    HANDLE hThrd[5];
    DWORD threadId;
    int i;
    LinkedList* pList = createList();
    clock_t start = clock();

    for (i = 0; i < 5; i++)
    {
        hThrd[i] = CreateThread(NULL,//스레드KO(kernel object) 생성 후 핸들을 반환한다.
            0,
            ThreadFunc,//함수 포인터
            (LPVOID)pList,//함수의 매개변수, 단 하나만 허락
            0,
            &threadId);
        if (hThrd[i])
        {
            printf("Thread launched %d\n", i);
            //CloseHandle(hThrd);
        }
        else { //스레드 생성에 실패하면 hThrd == NULL
            printf("error creating a thread\n ");
            return 0;
        }
    }
    WaitForMultipleObjects(5, hThrd, TRUE, INFINITE);//모든 스레드가 종료되어야 넘어간다.
    for (i = 0; i < 5; i++) {
        CloseHandle(hThrd[i]);//1.스레드 실행 종료 2. CloseHandle() 스레드 KO를 소멸
    }
    //ThreadFunc(&pHead);
    printf("the number of Nodes is %d\n", countNode(pList));
    clock_t end = clock();
    double time = double(end - start) / CLOCKS_PER_SEC; //초단위 변환
    printf("경과시간 : %0.3lf\n", time); //소수점 셋째 자리까지
    return EXIT_SUCCESS;
}

DWORD WINAPI ThreadFunc(LPVOID ptr)
{

    LinkedList* pList = (LinkedList*)ptr;
    for (int i = 0; i < 1000000; i++)
        insertHead(pList, createNode(100));
    return 0;
}

