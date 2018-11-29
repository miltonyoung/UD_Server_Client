#ifndef UDP_SEND_H
#define UDP_SEND_H

//#define _WINSOCK_DEPRECATED_NO_WARNINGS
//#define _CRT_SECURE_NO_WARNINGS
#define BUFLEN 65536	//Max length of buffer

//------------------------------------------Includes-----------------------------------------------//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include<stdio.h>
#include<winsock2.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <QtDebug>
//-----------------------------------End of Includes-----------------------------------------------//

using namespace std;

//-------------------------------------Structures--------------------------------------------//
struct transcribedMessage
{
    string src;
    string dest;
    string msgType;
    string token;
    string msgID;
    string msg;
};
struct transmissionParams
{
    string transmissionType;
    int paramCount;
    string format;

};

typedef transmissionParams transmissionParams_t;
typedef transcribedMessage transcribedMessage_t;
//------------------------------------End of Structures--------------------------------------//

// Function Protoype
transcribedMessage_t processIncomingTransmission(char recv_buffer[1024]);


// Class Declaration:
// This class directly manages the socket communication for the client.
class SocketConnection
{
private:
    int sockfd_server; // Socket file descriptor for server
    int sockfd_client; // Socket file descriptor for server
    int maxfd;
    struct sockaddr_in servaddr;
    struct sockaddr_in cliaddr;
    string token; // token assigned to user at every login
	string userName;
	string password;

public:
    // Constructors
    SocketConnection();
    SocketConnection(string clientIPAddress, int clientPortNumber, string serverIPAddress, int serverPortNumber);

    // Socket Action Functions
    int logIn(string userName, string password);
    int sendMessage(string destClientID, string message);
    int recvMessageHandle(transcribedMessage_t transMsg);
    int logOut(string destClientID);

    // Mutator Functions
    void setToken(string t);
    void setUserName(string newUserName);
    void setPassword(string newPassword);

    // Accessor Functions
    int getSockfd_Server();
    int getSockfd_Client();
    int getMaxfd();
	struct sockaddr_in getServaddr();
    struct sockaddr_in getCliaddr();
    string getToken();
    string getClientIPAddress();
    int getClientPortNumber();
    string getServerIPAddress();
    int getServerPortNumber();
    string getUserName();
    string getPassword();



	
};





#endif // UDP_SEND_H
