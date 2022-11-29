// 20211124iobyfile.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
/*
 * IoByFile.c
 *
 * Sample code for Multithreading Applications in Win32
 * This is from Chapter 6, Listing 6-1
 *
 * Demonstrates how a file handle becomes signaled
 * when an overlapped operation on the handle's file
 * is completed.  Notice that sometimes the operation
 * is completed immediately. The operation is only
 * queued, or pended, if ERROR_IO_PENDING is returned.
 */

 //#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "MTVERIFY.h"

 //
 // Constants
 //
#define MAX_REQUESTS    5
#define READ_SIZE       512
#define MAX_TRY_COUNT   5

//
// Function prototypes
//
int QueueRequest(int nIndex, DWORD dwLocation, DWORD dwAmount);

//
// Global variables
//
HANDLE  ghEvents[MAX_REQUESTS];
HANDLE hFile;
OVERLAPPED gOverlapped[MAX_REQUESTS];
char gBuffers[MAX_REQUESTS][READ_SIZE];


int main()
{
    int i;
    BOOL rc;
    
    DWORD numread;
    OVERLAPPED overlap;
    char buf[READ_SIZE];
    WCHAR szPath[MAX_PATH] = L"c:\\test.mp4";//200mbyte 이상
    //char szPath[MAX_PATH];

    hFile = CreateFile(szPath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,//앞으로 모든 입출력은 비동기로 수행
        NULL
    );
    if (hFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not open %s\n", szPath);
        return -1;
    }
    //나누어서 읽기
    for (i = 0; i < MAX_REQUESTS; i++)
    {
        // Read some bytes every few K
        QueueRequest(i, i * 16384, READ_SIZE);
    }

    MTVERIFY(WaitForMultipleObjects(
        MAX_REQUESTS, ghEvents, TRUE, INFINITE
    ) != WAIT_FAILED);

    for (i = 0; i < MAX_REQUESTS; i++)
    {
        DWORD dwNumread;

        rc = GetOverlappedResult(
            hFile,
            &gOverlapped[i],
            &dwNumread,
            FALSE
        );
        printf("Read #%d returned %d. %d bytes were read.\n",
            i, rc, dwNumread);
        CloseHandle(gOverlapped[i].hEvent);
    }


    CloseHandle(hFile);

    return EXIT_SUCCESS;
}

/*
 * Call ReadFile to start an overlapped request.
 * Make sure we handle errors that are recoverable.
 * Properly set up the event object for this operation.
 */
int QueueRequest(int nIndex, DWORD dwLocation, DWORD dwAmount)
{
    int i;
    BOOL rc;
    DWORD dwNumread;
    DWORD err;

    MTVERIFY(
        ghEvents[nIndex] = CreateEvent(
            NULL,    // No security
            TRUE,    // Manual reset - extremely important!
            FALSE,   // Initially set Event to non-signaled state
            NULL     // No name
        )
    );
    gOverlapped[nIndex].hEvent = ghEvents[nIndex];
    gOverlapped[nIndex].Offset = dwLocation;

    for (i = 0; i < MAX_TRY_COUNT; i++)
    {
        rc = ReadFile(
            hFile,
            gBuffers[nIndex],
            dwAmount,
            &dwNumread,
            &gOverlapped[nIndex]
        );

        // Handle success
        if (rc)
        {
            printf("Read #%d completed immediately.\n", nIndex);
            return TRUE;
        }

        err = GetLastError();

        // Handle the error that isn't an error. rc is zero here.
        if (err == ERROR_IO_PENDING)
        {
            // asynchronous i/o is still in progress 
            printf("Read #%d queued for overlapped I/O.\n", nIndex);
            return TRUE;
        }

        // Handle recoverable error
        if (err == ERROR_INVALID_USER_BUFFER ||
            err == ERROR_NOT_ENOUGH_QUOTA ||
            err == ERROR_NOT_ENOUGH_MEMORY)
        {
            Sleep(50);  // Wait around and try later
            continue;
        }

        // Give up on fatal error.
        break;
    }

    printf("ReadFile failed.\n");
    return -1;
}

