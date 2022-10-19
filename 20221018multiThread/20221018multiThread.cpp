#define WIN32_LEAN_AND_MEAN //??

#include <stdio.h>
#include <stdlib.h>
#include <windows.h> //windows API를 사용하려면 반드시 포함

DWORD WINAPI ThreadFunc(LPVOID);// LPVOID ==> void*
//대부분의 대문자 단어는 마이크로소프트사가 정의한 자료형이다.

typedef struct complex {
    double real;
    double image;
} Complex;

int main()
{
    HANDLE hThrd[5];
    //HANDLE hThrd;
    DWORD threadId;
    DWORD retVal;
    int i;
    Complex* ptr;
    ptr = (complex*)malloc(sizeof(Complex));
    /*ptr->real = 1.0;
    ptr->image = 2.0;*/

    for (i = 0; i < 5; i++) //loop condition : iterate 5 times
    {
        //ptr->real = i; ptr->image = i;
        //function CreateThread() returns NULL if it fails.
        hThrd[i] = CreateThread(NULL, //security attribute
            0, //size of stack, 1MB if 0
            ThreadFunc,//function pointer
            (LPVOID)i,//function parameter, only one is allowed
            //(LPVOID)ptr, //complex number를 인수로 전달
            0, //control the creation of thread by value
            //CREATE_SUSPENDED,
            &threadId); //thread id's address

        if (hThrd[i])
        {
            printf("Thread launched %d\n", i);
            //CloseHandle(hThrd); // 핸들은 없어지지만, KO는 살아있다. 해당 Thread가 종료되어야 KO또한 없어진다.
        }
    }
    // Wait for the threads to complete.
    // We'll see a better way of doing this later.
    //Sleep(2000);//2000->20000
    //ResumeThread(hThrd);
    //Sleep(2000);
    retVal = WaitForMultipleObjects(5, //number of threads
        hThrd, //An array of object handles
        TRUE, //If this parameter is TRUE, the function returns when the state of all objects in the lpHandles array is signaled
        INFINITE//5개의 Thread가 모두 종료(스레드 객체가 모두 종료 Signaled State)할 때까지 기다린다.
    );
    //retVal = WaitForMultipleObjects(5, //number of threads
    //    hThrd, //An array of object handles
    //    TRUE, //If this parameter is TRUE, the function returns when the state of all objects in the lpHandles array is signaled
    //    1
    //);
    //printf("retvalue = %d\n", retVal-WAIT_OBJECT_0);
    if (retVal == WAIT_TIMEOUT)
        printf("timeout !!!\n");
    else if(retVal == WAIT_OBJECT_0 + 4)
        printf("all signaled !!!\n");
    else
        printf("serious problem !!!\n");



    return EXIT_SUCCESS;
}

DWORD WINAPI ThreadFunc(LPVOID in)
{
    int i;
    //Complex* ptr = (Complex*)in;//입력 패러미터 형변환
    int n = (int)in;
    for (i = 0; i < 100; i++)//10->100
        printf("%d%d%d%d%d%d%d%d\n", n, n, n, n, n, n, n, n);
        //printf("%f %f %f\n", ptr->real,ptr->real,ptr->image);
    return 0;
}