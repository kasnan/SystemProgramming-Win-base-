﻿// 20221206ICPEchoClient.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
/*
 * EchoCli.c
 *
 * Sample code for "Multithreading Applications in Win32"
 * This is from Chapter 6.
 *
 * This is a command line client to drive the ECHO server.
 * Run the server in one Commmand Prompt Window,
 * then run this program in one or more other windows.
 * EchoCli will wait for you to type in some text when
 * it starts up. Each line of text will be sent to the
 * echo server on TCP port 5554.
 */

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock.h>
 // Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

 /* Function Prototypes */
void FatalError(const char* s);
int writen(SOCKET sock, char* ptr, int nBytes);
int readline(SOCKET sock, char* ptr, int maxlen);
void DoClientLoop(FILE* fp, SOCKET sock);

/* Constants */
#define MAXLINE 512
#define SERVER_TCP_PORT		5554
#define SERVER_ADDRESS		"127.0.0.1"


int main(int argc, char* argv[])
{
    WSADATA WsaData;
    SOCKET sock;
    struct sockaddr_in  serv_addr;
    int err;

    puts("EchoCli - client for echo server sample program\n");
    puts("Type a line of text followed by Return.");
    puts("Exit this program by typing Ctrl+Z followed by Return.");

    err = WSAStartup(0x0101, &WsaData);
    if (err == SOCKET_ERROR)
        FatalError("WSAStartup Failed");

    /*
     * Bind our local address
     */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    // Use the local host
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
    serv_addr.sin_port = htons(SERVER_TCP_PORT);

    /*
     * Open a TCP socket (an Internet stream socket)
     */

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        FatalError("socket() failed -- do you have TCP/IP networking installed?");

    if (connect(sock, (struct sockaddr*)&serv_addr,
        sizeof(serv_addr)) < 0)
        FatalError("connect() failed -- is the server running?");

    DoClientLoop(stdin, sock);

    closesocket(sock);

    return EXIT_SUCCESS;
}

/*
 * As long as there is input from "fp", copy it to
 * the server, read what the server gives back,
 * and print it.
 */
void DoClientLoop(FILE* fp, SOCKET sock)
{
    int n;
    char sendline[MAXLINE];
    char recvline[MAXLINE + 1];

    while (fgets(sendline, MAXLINE, fp) != NULL)
    {
        n = strlen(sendline);
        if (writen(sock, sendline, n) != n)
            FatalError("DoClientLoop: writen() error");

        n = readline(sock, recvline, MAXLINE);
        if (n < 0)
            FatalError("DoClientLoop: readline() error");
        recvline[n] = '\0';
        fputs(recvline, stdout);
    }

    if (ferror(fp))
        FatalError("DoClientLoop: error reading file");
}


/*
 * Error handler
 */
void FatalError(const char* s)
{
    fprintf(stdout, "%s\n", s);
    exit(EXIT_FAILURE);
}

/*
 * Write bytes to the port with proper block size handling.
 */
int writen(SOCKET sock, char* ptr, int nBytes)
{
    int nleft;
    int nwritten;

    nleft = nBytes;
    while (nleft > 0)
    {
        nwritten = send(sock, //returns total number of bytes sent
            ptr, //buffer
            nBytes, //number of bytes
            0
        );

        if (nwritten == SOCKET_ERROR)
        {
            fprintf(stdout, "Send Failed\n");
            exit(1);
        }

        nleft -= nwritten;
        ptr += nwritten;
    }

    return nBytes - nleft;
}

/*
 * Read a line of text of the port. This version
 * is very inefficient, but it's simple.
 */
int readline(SOCKET sock, char* ptr, int maxlen)
{
    int n;
    int rc;
    char c;

    for (n = 1; n < maxlen; n++)
    {
        rc = recv(sock, &c, 1, 0);
        if (rc >= 1)
        {
            *ptr++ = c;
            if (c == '\n')
                break;
        }
        else if (rc == 0)
        {
            if (n == 1)
                return 0;
            else
                break;
        }
        else {
            int err = WSAGetLastError();
            printf("err: %d\n", err);
            printf("rc : %d\n", rc);
            return -1;  /* Error */
        }
    }

    *ptr = '\0';
    return n;
}
