#include <stdio.h>
#include <Winsock2.h>
#include<iostream>
#include<WS2tcpip.h>
#pragma comment(lib, "WS2_32")  // 链接到WS2_32.lib
#define PORT 5001
#define CMDBUFSIZE 100

using namespace std;
string recvMessage(SOCKET sockConn);
void main()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(1, 1);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0) {
        return;
    }

    if (LOBYTE(wsaData.wVersion) != 1 ||
        HIBYTE(wsaData.wVersion) != 1) {
        WSACleanup();
        return;
    }
    SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);

    SOCKADDR_IN addrSrv;
    addrSrv.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    addrSrv.sin_family = AF_INET;
    addrSrv.sin_port = htons(PORT);

    bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));

    listen(sockSrv, 5);

    SOCKADDR_IN addrClient;
    int len = sizeof(SOCKADDR);
    while (1)
    {
        SOCKET sockConn = accept(sockSrv, (SOCKADDR*)&addrClient, &len);
        
        while(true) {//keep controlling
            string curP = recvMessage(sockConn);
            if (curP.length() == 0){
                cout << "server closed" << endl;
                break;
            }
            
            //int sockCloseFlag = 0;
            char sendBuf[CMDBUFSIZE];
            //cout << "input your command:\t";
            while (true) {
                cout << curP << ">";
                if (!cin.getline(sendBuf, CMDBUFSIZE)) {
                    cout << "cmd length too large!" << endl;
                    continue;
                }
                if (strlen(sendBuf) == 0) {
                    continue;
                }
                break;
            }
            
            
            //send control message
            
            send(sockConn, sendBuf, strlen(sendBuf), 0);

           
            string reply = recvMessage(sockConn);
            /*if (reply._Equal("")) {
                cout << "server closed" << endl;
                break;
            }*/
            //int l = 24;
            //reply = reply.substr(l, reply.length() - l);
            cout << reply << endl;
        }
        closesocket(sockConn);
    }
}
string recvMessage(SOCKET sockConn) {
    //cout << "recvMessage start" << endl;
    string reply = "";
    const int BUFFER_SIZE = 50;
    char recvBuf[BUFFER_SIZE];
    memset(recvBuf, 0, BUFFER_SIZE);
    //receive the message length
    int recvLength = recv(sockConn, recvBuf, 10, 0);

    if (recvLength <= 0) {
        return "";//sock closed
    }
    int messageLength = atoi(recvBuf);
    //cout << "recvlength:" << recvBuf << endl;
    //recursive receive long message
    while (true) {
        if (messageLength <= 0) {
            break;
        }
        memset(recvBuf, 0, BUFFER_SIZE);
        int recvBufferSize = messageLength;
        if (recvBufferSize > BUFFER_SIZE - 1) {
            recvBufferSize = BUFFER_SIZE - 1;
        }
        recvLength = recv(sockConn, recvBuf, recvBufferSize, 0);
        if (recvLength == 0) {
            return "";
        }
        reply += recvBuf;
        messageLength -= recvLength;
    };
    
    return reply;
}