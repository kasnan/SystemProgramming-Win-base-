﻿// testDeadlock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
//**************************************************************
// 제목: Dead Lock
// 기능: swap list함수에 의한 dead lock 데모
// 파일이름: deadlock.cpp
// 수정날짜: 2022년11월8일
// 작성자: 최성종
//************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
typedef struct node {
	int data;
	struct node* pNext;
} Node;
typedef struct linkedList {
	Node* pHead;
	CRITICAL_SECTION critical_sec;
	int dummy;
} LinkedList;

//함수 printLL()
//입력: Linked List의 포인터
//출력: 없음
//부수효과: 화면에 Linked List 데이터 값을 출력한다.
void printLL(LinkedList* pLL) {
	Node* pNode;
	pNode = pLL->pHead;
	while (pNode != NULL) {
		printf("data = %d\n", pNode->data);
		pNode = pNode->pNext;
	}
}
int countNode(LinkedList* pLL) {
	Node* pNode;
	pNode = pLL->pHead;
	int num = 0;
	while (pNode != NULL) {
		num++;
		pNode = pNode->pNext;
	}
	return num;
}
//함수 createNode()
//입력: 데이터(int)
//출력: 노드의 포인터
//부수효과: 동적 메모리할당을 사용하여 하나의 노드를 생성
Node* createNode(int num) {
	Node* pNode;
	pNode = (Node*)malloc(sizeof(Node));
	pNode->data = num;
	pNode->pNext = NULL;
	return pNode;
}
//함수 addHead()
//입력: linked list(포인터), 노드(포인터)
//출력: 없음
//부수효과: linked list 맨 앞에 입력한 노드를 삽입
void addHead(LinkedList* pLL, Node* pNode) {
	EnterCriticalSection(&pLL->critical_sec);
	pNode->pNext = pLL->pHead;
	pLL->pHead = pNode;
	LeaveCriticalSection(&pLL->critical_sec);
}
//함수 createLinkedList()
//입력: 없음
//출력: linked list(포인터)
//부수효과: 동적메모리할당으로 linked list공간 사용
LinkedList* createLinkedList() {
	LinkedList* pLL;
	pLL = (LinkedList*)malloc(sizeof(LinkedList));
	pLL->pHead = NULL;
	InitializeCriticalSection(&pLL->critical_sec);
	return pLL;
}
//함수 deleteHead()
//입력: Linked List(포인터)
//출력: 없음
//부수효과: linked list의 맨 처음 노드를 삭제
void deleteHead(LinkedList* pLL) {
	Node* pNode = pLL->pHead;
	if (pLL->pHead == NULL) return;
	pLL->pHead = pLL->pHead->pNext;
	free(pNode);
}
void swapLists(LinkedList* list1, LinkedList* list2, int thread) {
	LinkedList* tmp_list = createLinkedList();
	EnterCriticalSection(&list1->critical_sec);//possible context switch
	printf("========between line in thread %d\n ", thread);
	EnterCriticalSection(&list2->critical_sec);
	tmp_list->pHead = list1->pHead;
	list1->pHead = list2->pHead;
	list2->pHead = tmp_list->pHead;
	LeaveCriticalSection(&list1->critical_sec);
	LeaveCriticalSection(&list2->critical_sec);
}

LinkedList* pLL1, * pLL2;//global for demo's sake!!! sorry
DWORD WINAPI ThreadFunc1(LPVOID n) {
	while (1) {
		printf("begin swapping in Thread11111\n");
		swapLists(pLL1, pLL2, 1);
		printf("end swapping in Thread11111\n");
	}
	return 0;
}
DWORD WINAPI ThreadFunc2(LPVOID n) {
	while (1) {
		printf("begin swapping in Thread22222\n");
		swapLists(pLL2, pLL1, 2);
		
		printf("end swapping in Thread22222\n");
	}
	return 0;
}
int main() {
	HANDLE hThrd[5];
	DWORD threadId;
	int i = 0;
	pLL1 = createLinkedList();
	pLL2 = createLinkedList();
	addHead(pLL1, createNode((int)1));//pLL1은 data가 1인 노드 하나를 가진다
	addHead(pLL2, createNode((int)2));//pLL2는 data가 2인 노드 하나를 가진다
	printLL(pLL1);// 출력: data = 1 
	swapLists(pLL1, pLL2, 100);
	printLL(pLL1);// 출력: data = 2

	hThrd[0] = CreateThread(NULL, 0, ThreadFunc1, (LPVOID)i, 0, &threadId);
	hThrd[1] = CreateThread(NULL, 0, ThreadFunc2, (LPVOID)i, 0, &threadId);

	WaitForMultipleObjects(2, hThrd, true, INFINITE);
	for (i = 0; i < 2; i++) {
		CloseHandle(hThrd[i]);
	}
	printf("End of Program!!!\n");

	return EXIT_SUCCESS;
}