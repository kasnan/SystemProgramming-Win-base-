﻿#include<stdio.h>
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
	Node a;
	//struct node* pNode; //구조체 포인터 변수
	Node* pNode;
	myType n;// -> int n
	a.data = 10;//a의 멤버 변수 data에 다시 접근해 10으로 초기화
	a.pNext = NULL;

	pNode = &a; //&a = address of a
	printf("%d\n", pNode->data);//a의 멤버 변수 data를 접근하는 또다른 방법


	
	return 0;
}