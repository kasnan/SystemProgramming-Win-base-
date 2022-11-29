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
#define READ_SIZE       512

//
// Function prototypes
//
int QueueRequest(DWORD dwAmount);

//
// Global variables
//

// Need to keep the events in their own array
// so we can wait on them.
HANDLE  ghEvents;
// Keep track of each individual I/O operation
OVERLAPPED gOverlapped;
// Handle to the file of interest.
HANDLE ghFile;
// Need a place to put all this data
char gBuffers[READ_SIZE];

int main()
{
    DWORD dwNumread;
    WCHAR szPath[MAX_PATH] = L"c:\\test.mp4";//200mbyte 이상
    //char szPath[MAX_PATH];

    ghFile = CreateFile(szPath,
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_OVERLAPPED,//앞으로 모든 입출력은 비동기로 수행
        NULL
    );
    if (ghFile == INVALID_HANDLE_VALUE)
    {
        printf("Could not open %s\n", szPath);
        return -1;
    }

    QueueRequest(READ_SIZE);

    MTVERIFY(WaitForSingleObject(
        ghEvents,INFINITE) != WAIT_FAILED);

    DWORD rc = GetOverlappedResult(
        ghFile,
        &gOverlapped,
        &dwNumread,
        FALSE
    );
    printf("Read returned %d. %d bytes were read.\n",
        rc, dwNumread);
    CloseHandle(gOverlapped.hEvent);

    CloseHandle(ghFile);

    return EXIT_SUCCESS;
}


int QueueRequest(DWORD dwAmount)
{
    BOOL rc;
    DWORD dwNumread;
    DWORD err;

    MTVERIFY(
        ghEvents = CreateEvent(
            NULL,    // No security
            TRUE,    // Manual reset - extremely important!
            FALSE,   // Initially set Event to non-signaled state
            NULL     // No name
        )
    );
    gOverlapped.hEvent = ghEvents;
    gOverlapped.Offset = 0;

    rc = ReadFile(
        ghFile,
        gBuffers,
        dwAmount,
        &dwNumread,
        &gOverlapped
    );
    printf("Issued read request\n");
    // Handle success
    if (rc)
    {
        printf("Request was returned immediately\n");
        return TRUE;
    }

    err = GetLastError();

    // Handle the error that isn't an error. rc is zero here.
    if (err == ERROR_IO_PENDING)
    {
        // asynchronous i/o is still in progress 
        printf("Read for overlapped I/O.\n");
        return TRUE;
    }

    // Handle recoverable error
    if (err == ERROR_INVALID_USER_BUFFER ||
        err == ERROR_NOT_ENOUGH_QUOTA ||
        err == ERROR_NOT_ENOUGH_MEMORY)
    {
        printf("Recoverable Error Reading File!\n");
        Sleep(50);
    }
    printf("ReadFile failed.\n");
    return -1;
}
