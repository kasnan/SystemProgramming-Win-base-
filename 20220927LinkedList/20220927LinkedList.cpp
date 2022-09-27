#include<stdio.h>
#include<stdlib.h>

//struct node {
//	int data;//data filed, 멤버변수
//	struct node* pNext;//link field, 
//} ;

typedef struct node {
	int data;//data filed, 멤버변수
	struct node* pNext;//link field, 
} Node;


//typedef struct node Node;//앞으로 Node는 struct node이다.
typedef int myType; //앞으로 myType이라는 자료형은 int형이다.
int main() {
	//struct node a; //구조체 변수
	//Node a;
	//struct node* pNode; //구조체 포인터 변수
	Node* pNode, * pNode1, * pHead;
	myType n;// -> int n
	static int a;
	pNode = (Node *)malloc(sizeof(Node)); // malloc(원하는 Byte 수) heap 영역
	pNode->data = 10; //멤버접근 heap 영역
	pNode->pNext = NULL; //멤버접근 heap 영역
	//a.data = 10;//a의 멤버 변수 data에 다시 접근해 10으로 초기화
	//a.pNext = NULL;

	pNode1 = (Node*)malloc(sizeof(Node)); // malloc(원하는 Byte 수) heap 영역
	pNode1->data = 100; //멤버접근 heap 영역
	pNode1->pNext = pNode; //멤버접근 heap 영역
	pHead = pNode1;

	//pNode = &a; //&a = address of a
	//printf("%d\n", pNode->data);//a의 멤버 변수 data를 접근하는 또다른 방법

	pNode = pHead;// 1.loop 변수 초기화
	while (pNode != NULL) { // 2.loop 조건식
		printf("data = %d\n", pNode->data); // 3.loop 문장
		pNode = pNode->pNext; // 4.loop변수 변화
	}

	return 0;
}