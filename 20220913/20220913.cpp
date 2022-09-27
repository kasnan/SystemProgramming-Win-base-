//제목: 문자열 출력 및 변수 선언

//기능: 문자열출력 및 변수 선언
//파일이름: hello.cpp
//수정날짜: 2022년9월13일
//작성자: 김도현

#include<stdio.h>

int main() {

	int a, b, c;//변수선언문 : 변수이름 및 자료형
 	a = 10;//대입연산자 : 우변의 값을 좌변에 저장
	b = 0x123445678;//리터럴(hexadecimal)
	c = a + b;	//산술연산자
	printf("Hello World!\n");

	return 0;
}
