// 20220920multifile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include<stdio.h>

int sum(int a, int b);

int main() {

	int a, b, c;//변수선언문 : 변수이름 및 자료형
	a = 10;//대입연산자 : 우변의 값을 좌변에 저장
	b = 0x123445678;//리터럴(hexadecimal)
	c = sum(a, b);
	printf("c = %d\n",c);

	return 0;
}