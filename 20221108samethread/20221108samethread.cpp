// 20211117samethread.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
//2021.11.17 시스템프로그래밍 W12-2
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

int main() {
	CRITICAL_SECTION cs1;
	InitializeCriticalSection(&cs1);//수행 후 깃발이 올라가 있음(signaled 들어올 수 있음)
	EnterCriticalSection(&cs1);//수행 후 깃발을 내림 (nonsignaled 못 들어옴)
	printf("we are taking cs1\n");
	EnterCriticalSection(&cs1);//동일 Thread 내에서 통과!
	printf("we are taking cs1?????\n");

	//Mutex
	HANDLE hM;
	hM = CreateMutex(NULL, FALSE, NULL); //signaled 초기화
	WaitForSingleObject(hM, INFINITE); //side Effect: ->nonsignaled
	printf("we are taking hM\n");
	WaitForSingleObject(hM, INFINITE); //동일 Thread 내에서 통과!
	printf("we are taking hM\n");

	return 0;
}
