
#ifndef UDP_SEND_H
#define UDP_SEND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;


class SocketConnection
{
private:
	int sockfd_server;
	int sockfd_client;
	int maxfd;
	struct sockaddr_in servaddr;
	struct sockaddr_in cliaddr;
	string token;
	string userName;
	string password;

public:
	SocketConnection(string clientIPAddress, int clientPortNumber, string serverIPAddress, int serverPortNumber);
	int logIn(string userName, string password);
	int sendMessage(string destClientID, string message);
	int getSockfd_Server();
	int getSockfd_Client();
	int getMaxfd();
	struct sockaddr_in getServaddr();
	struct sockaddr_in getCliaddr();
	string getToken();
	void setToken(string t);
	string getUserName();
	void setUserName(string newUserName);
	string getPassword();
	void setPassword(string newPassword);
	string getClientIPAddress();
	int getClientPortNumber();
	string getServerIPAddress();
	int getServerPortNumber();
	


};





#endif // UDP_SEND_H