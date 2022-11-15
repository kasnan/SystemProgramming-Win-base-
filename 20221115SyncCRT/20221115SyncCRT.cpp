#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>

int main() {
	const char* str = "JeongMinSeok"; // pointer로 문자열 선언 -> immutable object
	wchar_t* str2 = "정민석"; // string 끝에는 편하려고 \0 NULL문자를 추가.
	//UTF-16, UTF-8 : 인코딩 방식 - 바이트 크기만 다를 뿐.
	//str[0]='k';
	char str1[] = "Chois"; // array로 문자열 선언 -> mmutable object
	str1[0] = 'k';
	char arr[100];

	strcpy_s(arr, str);
	printf("%s", arr);
	return 0;
}