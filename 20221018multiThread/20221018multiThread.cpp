#define WIN32_LEAN_AND_MEAN //??
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> //windows API를 사용하려면 반드시 포함

DWORD WINAPI ThreadFunc(LPVOID);// LPVOID ==> void*
//대부분의 대문자 단어는 마이크로소프트사가 정의한 자료형이다.

int main()
{
    HANDLE hThrd;
    DWORD threadId;
    int i;

    for (i = 0; i < 5; i++) //loop condition : iterate 5 times
    {
        //function CreateThread() returns NULL if it fails.
        hThrd = CreateThread(NULL, //security attribute
            0, //size of stack, 1MB if 0
            ThreadFunc,//function pointer
            (LPVOID)i,//function parameter, only one is allowed
            0, //control the creation of thread by value
            &threadId); //thread id's address
        if (hThrd)
        {
            printf("Thread launched %d\n", i);
            CloseHandle(hThrd); // 생성된 커널 오브젝트를 소멸시킨다.
        }
    }
    // Wait for the threads to complete.
    // We'll see a better way of doing this later.
    Sleep(5000);//2000->20000

    return EXIT_SUCCESS;
}

DWORD WINAPI ThreadFunc(LPVOID in)
{
    int i;
    int n = (int)in;
    for (i = 0; i < 10; i++)//10->10000
        printf("%d%d%d%d%d%d%d%d\n", n, n, n, n, n, n, n, n);
    return 0;
}