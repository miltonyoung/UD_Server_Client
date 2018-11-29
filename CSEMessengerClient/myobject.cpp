#include "myobject.h"
#include <string>
#include <iostream>
#include <pthread.h>
#include <QtDebug>

MyObject::MyObject(){socketConnection = new SocketConnection();}

SocketConnection* MyObject::getSocketCommunicationObj()
{
    return socketConnection;
}

void MyObject::initializeSocketConnection(QString clientIPAddress, QString clientPortNumber, QString serverIPAddress, QString serverPortNumber)
{
    socketConnection = new SocketConnection(clientIPAddress.toStdString(), stoi(clientPortNumber.toStdString()), serverIPAddress.toStdString(), stoi(serverPortNumber.toStdString()));
}
void MyObject::logIn(QString userName, QString password)
{
    socketConnection->logIn(userName.toStdString(), password.toStdString());
}
void MyObject::logOut(QString userName)
{
    socketConnection->logOut(userName.toStdString());
}
void MyObject::sendMessage(QString destClientID, QString message)
{
    socketConnection->sendMessage(destClientID.toStdString(), message.toStdString());
}
QString MyObject::getResponse() { return response;}

// Function will be running as a thread in the main.cpp
void* startEventLoop(void* arg)
{
    threadInfo_t info = *((threadInfo_t*)arg);
    char recv_buffer[BUFLEN];
    fd_set read_set;
    FD_ZERO(&read_set);

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; // 100 ms

    struct sockaddr_in servaddr = info.sockConn->getServaddr();
    int servaddrLen = sizeof(servaddr);

    while (1)
    {
       // If the socketConnection has not been initialized, continue to the new loop iteration.
       info.sockConn = info.myObj->getSocketCommunicationObj();
        if(info.sockConn->getSockfd_Server() == -1)
            continue;


        fflush(stdout);
        FD_SET(info.sockConn->getSockfd_Server(), &read_set);
        select(info.sockConn->getMaxfd(), &read_set, NULL, NULL, &timeout);

        // If the server socket has data waiting, read and process it.
        if (FD_ISSET(info.sockConn->getSockfd_Server(), &read_set))
        {
            memset(recv_buffer, '\0', BUFLEN);
            //try to receive some data, this is a blocking call
            if(recvfrom(info.sockConn->getSockfd_Server(), recv_buffer, BUFLEN, 0, (struct sockaddr *) &(servaddr), &servaddrLen) == SOCKET_ERROR)
            {
                printf("recvfrom() failed with error code : %d", WSAGetLastError());
                continue;
            }
            transcribedMessage_t transMsg = processIncomingTransmission(recv_buffer);
            if(transMsg.msgType == "Authentication-Successful" )
            {
                info.myObj->getSocketCommunicationObj()->setToken(transMsg.token);
            }
            if(transMsg.msgType == "Message" )
            {
                info.myObj->getSocketCommunicationObj()->recvMessageHandle(transMsg);
            }

            info.myObj->emitSignal(QString::fromStdString((string(recv_buffer))));
        }


    }

    closesocket(info.sockConn->getSockfd_Server());
    WSACleanup();

}

void MyObject::emitSignal(QString response)
{
    emit newResponseReceived(response);
}


