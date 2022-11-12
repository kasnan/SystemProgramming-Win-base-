// testDeadlock.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
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
typedef struct List {
	Node* pHead;
	//CRITICAL_SECTION critical_sec;
	//HANDLE hMutex;
	int dummy;
} List;
typedef struct Listpair {
	List* pLL1;
	List* pLL2;
	HANDLE hMutex;
} Listpair;

//함수 printLL()
//입력: Linked List의 포인터
//출력: 없음
//부수효과: 화면에 Linked List 데이터 값을 출력한다.
void printLL(List* pLL) {
	Node* pNode;
	pNode = pLL->pHead;
	while (pNode != NULL) {
		printf("data = %d\n", pNode->data);
		pNode = pNode->pNext;
	}
}
int countNode(List* pLL) {
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
void addHead(List* pLL, Node* pNode) {
	//EnterCriticalSection(&pLL->critical_sec);
	//WaitForSingleObject(pLL->hMutex, INFINITE);
	pNode->pNext = pLL->pHead;
	pLL->pHead = pNode;
	//ReleaseMutex(pLL->hMutex);
	//LeaveCriticalSection(&pLL->critical_sec);
}
//함수 createLinkedList()
//입력: 없음
//출력: linked list(포인터)
//부수효과: 동적메모리할당으로 linked list공간 사용
List* createLinkedList() {
	List* pLL;
	pLL = (List*)malloc(sizeof(List));
	pLL->pHead = NULL;
	//pLL->hMutex = CreateMutex(NULL, FALSE, NULL);
	//InitializeCriticalSection(&pLL->critical_sec);
	return pLL;
}
Listpair* initListPair() {
	Listpair* pLP;
	pLP = (Listpair*)malloc(sizeof(Listpair));
	pLP->pLL1 = NULL;
	pLP->pLL2 = NULL;
	pLP->pLL1 = createLinkedList();
	pLP->pLL2 = createLinkedList();

	pLP->hMutex = CreateMutex(NULL, FALSE, NULL);
	return pLP;
}
//함수 deleteHead()
//입력: Linked List(포인터)
//출력: 없음
//부수효과: linked list의 맨 처음 노드를 삭제
void deleteHead(List* pLL) {
	//WaitForSingleObject(pLL->hMutex, INFINITE);
	Node* pNode = pLL->pHead;
	if (pLL->pHead == NULL) return;
	pLL->pHead = pLL->pHead->pNext;
	free(pNode);
	//ReleaseMutex(pLL->hMutex);
}
void swapLists(Listpair* pLP, int thread) {
	List* tmp_list = createLinkedList();
	List* list1 = pLP->pLL1;
	List* list2 = pLP->pLL2;

	//EnterCriticalSection(&list1->critical_sec);//possible context switch
	//WaitForSingleObject(list1->hMutex, INFINITE);
	WaitForSingleObject(pLP->hMutex, INFINITE);
	printf("========between line in thread %d\n ", thread);
	//for (int i = 0; i < 1000; i++)
		//1 + 2 + 3;
	//EnterCriticalSection(&list2->critical_sec);
	//WaitForSingleObject(list2->hMutex, INFINITE);
	tmp_list->pHead = list1->pHead;
	list1->pHead = list2->pHead;
	list2->pHead = tmp_list->pHead;
	//LeaveCriticalSection(&list1->critical_sec);
	//ReleaseMutex(list1->hMutex);
	//LeaveCriticalSection(&list2->critical_sec);
	//ReleaseMutex(list2->hMutex);
	ReleaseMutex(pLP->hMutex);
}


//DWORD WINAPI ThreadFunc1(LPVOID n) {
//	while (1) {
//		printf("begin swapping in Thread11111\n");
//		swapLists(pLL1, pLL2, 1);
//		printf("end swapping in Thread11111\n");
//	}
//	return 0;
//}
//DWORD WINAPI ThreadFunc2(LPVOID n) {
//	while (1) {
//		printf("begin swapping in Thread22222\n");
//		swapLists(pLL2, pLL1, 2);
//
//		printf("end swapping in Thread22222\n");
//	}
//	return 0;
//}
DWORD WINAPI ThreadFuncnew1(LPVOID n) {
	Listpair* pLP = (Listpair*)n;
		while (1) {
		printf("begin swapping in Thread11111\n");
		swapLists(pLP, 1);
		printf("end swapping in Thread11111\n");
	}
	return 0;

}
DWORD WINAPI ThreadFuncnew2(LPVOID n) {
	Listpair* pLP = (Listpair*)n;
	while (1) {
		printf("begin swapping in Thread11111\n");
		swapLists(pLP, 2);
		printf("end swapping in Thread11111\n");
	}
	return 0;
}
int main() {
	HANDLE hThrd[5];
	DWORD threadId;
	int i = 0;
	Listpair* pLP = initListPair();
	
	addHead(pLP->pLL1, createNode((int)1));//pLL1은 data가 1인 노드 하나를 가진다
	addHead(pLP->pLL2, createNode((int)2));//pLL2는 data가 2인 노드 하나를 가진다

	printLL(pLP->pLL1);// 출력: data = 1 
	swapLists(pLP, 100);
	printLL(pLP->pLL1);// 출력: data = 2

	hThrd[0] = CreateThread(NULL, 0, ThreadFuncnew1, (LPVOID)pLP, 0, &threadId);
	hThrd[1] = CreateThread(NULL, 0, ThreadFuncnew2, (LPVOID)pLP, 0, &threadId);

	WaitForMultipleObjects(2, hThrd, true, INFINITE);
	for (i = 0; i < 5; i++) {
		CloseHandle(hThrd[i]);
	}
	printf("End of Program!!!\n");

	return EXIT_SUCCESS;
}