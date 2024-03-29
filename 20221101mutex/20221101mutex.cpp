﻿// 20211110mutex.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

// 20211110criticalsectionObject.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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

 //#define WIN32_LEAN_AND_MEAN
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
    HANDLE hMutex;//깃발 추가 : mutex handle
} List;
//함수: createList()
//입력: 없음
//출력: List 구조체의 객체를 생성하여 이 포인터를 반환
LinkedList* createList() {
    LinkedList* pList = (LinkedList*)malloc(sizeof(LinkedList));
    pList->pHead = NULL;
    pList->hMutex = CreateMutex(NULL, FALSE, NULL); //mutex 초기화
    //2번째 인수=FALSE -> 누구든지 가져가(signaled)
    //TRUE -> ,초기화와 동시에 내가 가져감(nonsignaled)
    return pList;
}
//함수: deleteList()
// 입력: 리스트 포인터
// 출력: 없음
void deleteList(LinkedList* pList) {
    CloseHandle(pList->hMutex);
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
void insertHead(LinkedList* pList, Node* newNode) {
    WaitForSingleObject(pList->hMutex, INFINITE);//깃발확인, 깃발 올려
    //위의 함수는 현재 뮤텍스가 nonsignaled상태인 경우,signaled 상태가 될 때까지 기다린다.
    //만약 뮤텍스의 상태가 signaled로 변하면, 
    //  1.다음 문장으로 넘어간다.
    //  2. 뮤텍스의 상태를 nonsignaled 상태로 만든다. (부수효과: Side Effect)
    newNode->pNext = pList->pHead; //#1
    pList->pHead = newNode; //#2
    ReleaseMutex(pList->hMutex);//뮤텍스의 상태를 signaled로 바꾼다.
}

DWORD WINAPI ThreadFunc(LPVOID);// LPVOID ==> void*

int main()
{
    HANDLE hThrd[5];
    DWORD threadId;
    int i;
    LinkedList* pList = createList();//primary thread에서 list 생성
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

