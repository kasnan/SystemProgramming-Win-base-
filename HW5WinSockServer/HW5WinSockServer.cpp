#undef UNICODE //W -> Unicode A -> ASCII-code : 해당 코드는 ASCII-code

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h> //winsock aou version 2
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib") //link할 때 default로는 Ws2_32.lib가 안들어가 추가하기 위한 전처리 코드
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015" //port number 지정

int __cdecl main(void)
{
    WSADATA wsaData; //wsa : winsocket api
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET; //socket 초기화
    SOCKET ClientSocket = INVALID_SOCKET; //socket 초기화

    struct addrinfo* result = NULL; //port, ip address, ip name 등의 정보를 포함
    struct addrinfo hints;

    int iSendResult;
    int err;

    char sendbuf[] = "req";
    char recvbuf[DEFAULT_BUFLEN] = "";
    bool isCheck;
    
    //char* recvbuf;


    //winsock을 사용하기 위해 맨 처음 실행해야 한다. : Startup
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //malloc할당 hint에 메모리 크기 할당
    hints.ai_family = AF_INET; //ip_protocol을 사용
    hints.ai_socktype = SOCK_STREAM; //tcp(connection-oriented) 사용
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE; //server용 즉, 잠시후 bind() 사용

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result); //result에 정보 집어넣기
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    //서버용 소켓 생성
    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    // 주소 구조체 내부에 서버용 포트 번호 설정
    // 소켓에 기다리는 번호를 저장
    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    //최대 연결 가능한 세션 수(SOMAXCONN) 지정
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    //클라이언트로부터 연결 요청 기다리는 중
    // 연결 요청이 오면 새로운 소켓을 생성하여 이 소켓으로 데이터 송수신
    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    printf("accepted!\n");
    // listening socket닫기
    // 연결을 복수로 계속 받는 경우 listensocket을 닫으면 안된다.
    // No longer need server socket
    closesocket(ListenSocket);
    // 클라이언트와 데이터 교환
    // Receive until the peer shuts down the connection
    while (true){
        OVERLAPPED SocOverlapped = {};
        //숙제 : recv, send -> readfile, writefile로 변경하기
        // 상대방에게서 연결 종료 요청이 오면 0을 반환한다.
        
 
        printf("read socket\n");
        isCheck = ReadFile((HANDLE)ClientSocket, (LPVOID)recvbuf, (DWORD)DEFAULT_BUFLEN,
            NULL, &SocOverlapped);
        WaitForSingleObject((HANDLE)ClientSocket, INFINITE);
        
        err = WSAGetLastError();
        if (isCheck || err == WSA_IO_PENDING) {
            printf("Received Message : %s\n", recvbuf);
        }
        else {
            printf("Receive failed with error: %d\n", err);
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        
        err = 0;
        if (!strcmp(recvbuf, "z")) {
            isCheck = WriteFile((HANDLE)ClientSocket, (LPVOID)"readyforshutdown", (DWORD)DEFAULT_BUFLEN,
                NULL, &SocOverlapped);
            WaitForSingleObject((HANDLE)ClientSocket, INFINITE);
            err = WSAGetLastError();
            if (isCheck || err == WSA_IO_PENDING) {
                printf("Bytes Sent :%s\n", "readyforshutdown");
            }
            else {
                printf("send failed with error-code: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            break;
        }
        isCheck = WriteFile((HANDLE)ClientSocket, (LPVOID)sendbuf, (DWORD)DEFAULT_BUFLEN,
            NULL, &SocOverlapped);
        WaitForSingleObject((HANDLE)ClientSocket, INFINITE);
        err = WSAGetLastError();
        if (isCheck || err == WSA_IO_PENDING) {
            printf("Bytes Sent :%s\n", sendbuf);
        }
        else {
            printf("send failed with error-code: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
        
    } 

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    //맨 마지막은 소켓을 닫고 WSACleanup()을 호출해야 한다.
    // cleanup

    printf("Connection Closed!\n");
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}