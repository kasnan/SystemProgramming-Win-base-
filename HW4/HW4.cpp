#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "MTVERIFY.h"

 //
 // Constants
 //

#define READ_SIZE 512

//
// Function prototypes
//
int QueueRequest(DWORD dwAmount);

VOID WINAPI FileIOCompletionRoutine(
    DWORD dwErrorCode, // completion code 
    DWORD dwNumberOfBytesTransfered, // number of bytes transferred 
    LPOVERLAPPED lpOverlapped // pointer to structure with I/O information 
);

//
// Global variables
//

// Need a single event object so we know when all I/O is finished
HANDLE ghEvent;
// Keep track of each individual I/O operation
OVERLAPPED gOverlapped;
// Handle to the file of interest.
HANDLE ghFile;
// Need a place to put all this data
char gBuffers[READ_SIZE];
int nCompletionCount;

int main()
{
    WCHAR szPath[MAX_PATH] = L"c:\\test.mp4";//200mbyte 이상
    //char szPath[MAX_PATH];

    MTVERIFY(
        ghEvent = CreateEvent(
            NULL, // No security
            TRUE, // Manual reset - extremely important!
            FALSE, // Initially set Event to non-signaled state
            NULL // No name
        )
    );

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

    for (;;)
    {
        DWORD rc;
        rc = WaitForSingleObjectEx(ghEvent, INFINITE, TRUE);
        if (rc == WAIT_OBJECT_0) {
            printf("Read Completed!\n");
            break;
        }
        MTVERIFY(rc == WAIT_IO_COMPLETION);
    }
    CloseHandle(ghFile);

    return EXIT_SUCCESS;
}

int QueueRequest(DWORD dwAmount)
{
    BOOL rc;
    DWORD err;

    //callback함수에서 몇번째 불려진 ReadFileEx()함수인지를 알려 주기 위해
    //overlapped구조체의 이벤트핸들 값 대신 불려진 순서를 캐스팅한다.
    gOverlapped.hEvent = 0;
    gOverlapped.Offset = 0;

    rc = ReadFileEx(
        ghFile,
        gBuffers,
        dwAmount,
        &gOverlapped,
        FileIOCompletionRoutine
    );

    if (rc)
    {
        // asynchronous i/o is still in progress 
        printf("Read Returned immediately or Request queued for overlapped I/O.\n");
        WaitForSingleObject(ghFile, INFINITE);

        return TRUE;
    }

    err = GetLastError();

    // Handle recoverable error
    if (err == ERROR_INVALID_USER_BUFFER ||
        err == ERROR_NOT_ENOUGH_QUOTA ||
        err == ERROR_NOT_ENOUGH_MEMORY)
    {
        Sleep(50); // Wait around and try later
    }

    printf("ReadFileEx failed.\n");
    return -1;
}

VOID WINAPI FileIOCompletionRoutine(
    DWORD dwErrorCode, // completion code 
    DWORD dwNumberOfBytesTransfered, // number of bytes transferred 
    LPOVERLAPPED lpOverlapped // pointer to structure with I/O information 
)
{
    // The event handle is really the user defined data, 몇번째 불려진 ReadFileEx()인가.
    int nIndex = (int)(lpOverlapped->hEvent);
    printf("Read #%d returned %d. %d bytes were read.\n",
        nIndex,
        dwErrorCode,
        dwNumberOfBytesTransfered);

    if (++nCompletionCount == 1)
        SetEvent(ghEvent); // Cause the wait to terminate
}
