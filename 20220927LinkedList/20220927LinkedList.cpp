#include<stdio.h>
#include<stdlib.h>


struct node {
	int data;//data filed, 멤버변수
	struct node* pNext;//link field, 
};

int main() {
	struct node a;
	a.data = 10;
	a.pNext = NULL;

	
	return 0;
}