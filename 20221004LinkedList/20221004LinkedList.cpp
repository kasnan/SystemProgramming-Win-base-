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

//함수명 : printLL()
//입력 : 헤드노드포인터
//출력 : 없음
void printLL(Node* pHead) {
	Node* pNode = pHead;// 1.loop 변수 초기화
	while (pNode != NULL) { // 2.loop 조건식
		printf("data = %d\n", pNode->data); // 3.loop 문장
		pNode = pNode->pNext; // 4.loop변수 변화
	}

}

//함수명 : createNode()
//입력 : 데이터 값, 데이터 값은 입력값, pNext=NULL
//출력 : 노드를 생성해 그 포인터를 반환
Node* createNode(int value) {
	
	Node* pNode = (Node*)malloc(sizeof(Node)); // malloc(원하는 Byte 수) heap 영역

	/*Node node;
	Node* pNode = &node; stack 영역*/
	pNode->data = value;
	pNode->pNext = NULL;

	return pNode;
}

//함수명 : inserNode()
//입력 : 데이터 값, linked list의 Head 포인터
//출력 : 없음
void insertNode(int val, Node* pHead) {
	Node* pNode = createNode(val);
	pNode->pNext = pHead;  //#1
	pHead = pNode; //#2
	
	
}

int main() {
	//struct node a; //구조체 변수
	//Node a;
	//struct node* pNode; //구조체 포인터 변수
	Node* pNode, * pNode1, * pHead;

	pHead = createNode(10);
	insertNode(100, pHead);

	printLL(pHead);
	printf("Program End\n");

	return 0;
}