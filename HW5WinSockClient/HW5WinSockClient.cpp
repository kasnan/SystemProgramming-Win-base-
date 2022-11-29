﻿#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

int __cdecl main(int argc, char** argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    char sendbuf[DEFAULT_BUFLEN] = "a test";
    char recvbuf[DEFAULT_BUFLEN] = "";

    int iResult;
    bool isCheck;
    int recvbuflen = DEFAULT_BUFLEN;
    

    // Validate the parameters 패러미터를 더 입력!
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        //소켓 생성
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // 서버에 연결 요청
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
    printf("connected!\n");
    int err = 0;
    // Send a buffer
    do
    {
        OVERLAPPED SocOverlapped = {};
        isCheck = WriteFile((HANDLE)ConnectSocket, (LPVOID)sendbuf, (DWORD)DEFAULT_BUFLEN,
            NULL, &SocOverlapped);
        WaitForSingleObject((HANDLE)ConnectSocket, INFINITE);
        err = WSAGetLastError();
        if (isCheck || err == WSA_IO_PENDING) {
            printf("Bytes Sent :%s\n", sendbuf);
        }
        else {
            printf("send failed with error-code: %d\n", err);
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        if (!strcmp(sendbuf, "z")) {
            printf("final phase\n");
            isCheck = ReadFile((HANDLE)ConnectSocket, (LPVOID)recvbuf, (DWORD)DEFAULT_BUFLEN,
                NULL, &SocOverlapped);
            WaitForSingleObject((HANDLE)ConnectSocket, INFINITE);
            err = WSAGetLastError();
            if (isCheck || err == WSA_IO_PENDING) {
                printf("Final Received Message : %s\n", recvbuf);
            }
            else {
                printf("Final Receive failed with error: %d\n", err);
                closesocket(ConnectSocket);
                WSACleanup();
                return 1;
            }
            break;
        }

        isCheck = ReadFile((HANDLE)ConnectSocket, (LPVOID)recvbuf, (DWORD)DEFAULT_BUFLEN,
            NULL, &SocOverlapped);
        WaitForSingleObject((HANDLE)ConnectSocket, INFINITE);
        err = WSAGetLastError();
        if (isCheck || err == WSA_IO_PENDING) {
            printf("Received Message : %s\n", recvbuf);
        }
        else {
            printf("Receive failed with error: %d\n", err);
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        printf("type the message to send >> ");
        scanf("%s", sendbuf);
            

    } while (true);

    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while (iResult > 0);

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}