// 20211006mutithread.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
/*
 * Numbers.c
 *
 * Sample code for "Multithreading Applications in Win32"
 * This is from Chapter 2, Listing 2-1
 *
 * Starts five threads and gives visible feedback
 * of these threads running by printing a number
 * passed in from the primary thread.
 *
 */

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> // windows API를 사용하려면 반드시 포함

 //createThread함수에 의해 호출되는 함수는 
 //반드시 아래와 같은 함수형을 가져야 한다.
 //즉 1. 매개변수는 하나이고 자료형은 LPVOID 즉, void*
 //2.반환형 DWORD
 //3. calling convention(함수호출방법) WINAPI
DWORD WINAPI ThreadFunc(LPVOID);// LPVOID ==> typedef void* LPVOID , DWORD ==> typedef long DWORD
//대부분의 대문자 단어는 마이크로소프트사가 정의한 자료형이다.

#define SingleThread //전처리기로 조건 처리, 만약 SingleThread가 define되어 있으면

#ifdef SingleThread
int main() {
    ThreadFunc((LPVOID)10);

    return 0;
}

#else //그렇지 않으면
int main()
{
    HANDLE hThrd; //
    DWORD threadId;
    int i;

    for (i = 0; i < 5; i++)
    {
        hThrd = CreateThread(NULL,
            0,
            ThreadFunc,//함수 포인터
            (LPVOID)i,//함수의 매개변수, 단 하나만 허락
            0,
            &threadId);
        if (hThrd)
        {
            printf("Thread launched %d\n", i);
            CloseHandle(hThrd);
        }
    }
    // Wait for the threads to complete.
    // We'll see a better way of doing this later.
    //Sleep(2000); //wait for 2000ms
    Sleep(10000);

    return EXIT_SUCCESS;
}
#endif

DWORD WINAPI ThreadFunc(LPVOID in) //실제 작동 코드
{
    int i=-999999;
    int n = (int)in;
    while (1) {
        i += 10;
    }
    return 0;
}

