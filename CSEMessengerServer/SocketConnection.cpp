
#include "SocketConnection.h"


using namespace std;



SocketConnection::SocketConnection(string clientIPAddress, int clientPortNumber, string serverIPAddress, int serverPortNumber)
{
	socklen_t len;
	int ret;
	
    sockfd_server = socket(AF_INET, SOCK_DGRAM, 0);
	sockfd_client = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd_server < 0) 
	{
		printf("socket() error: %s.\n", strerror(errno));
        return;
    }
	
	if (sockfd_client < 0) 
	{
		printf("socket() error: %s.\n", strerror(errno));
        return;
    }

	// Server Address setup
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(serverIPAddress.c_str());
	servaddr.sin_port=htons(serverPortNumber); //destination port for incoming packets

	// Client Address Setup
	memset(&cliaddr, 0, sizeof(cliaddr));
	cliaddr.sin_family = AF_INET;
	cliaddr.sin_addr.s_addr= inet_addr(clientIPAddress.c_str());
	cliaddr.sin_port=htons(clientPortNumber); //source port for outgoing packets
	
	ret = connect(sockfd_server, (struct sockaddr *) &servaddr, sizeof(servaddr));
	if (ret < 0)
        return;
	
	ret = connect(sockfd_client, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
	if (ret < 0)
        return;

	maxfd = sockfd_server + 1;
}

int SocketConnection::logIn(string userName, string password)
{
	int ret;
	this->userName = userName;
	this->password = password;
	stringstream acc;
	
	acc << userName << "->server#[login]<" << password << "><" << getClientIPAddress() << "><" << getClientPortNumber() << ">";
	ret = send(sockfd_server, acc.str().c_str(), acc.str().length() + 1, 0);
	return ret;
}

int SocketConnection::sendMessage(string destClientID, string message)
{
	int ret;
	char responseArray[1000];
	memset(responseArray, 0, sizeof(responseArray));
	sprintf(responseArray, "%s->%s#[message]<%s><msg_id>%s", userName.c_str(), destClientID.c_str(), token.c_str(), message.c_str());
	ret = send(sockfd_server, responseArray, 1000, 0);
	return ret;
}

int SocketConnection::getSockfd_Client() {return sockfd_client;}
int SocketConnection::getSockfd_Server() {return sockfd_server;}
int SocketConnection::getMaxfd() {return maxfd;}
struct sockaddr_in SocketConnection::getServaddr() {return servaddr;}
struct sockaddr_in SocketConnection::getCliaddr() {return cliaddr;}
string SocketConnection::getToken() {return token;}
void SocketConnection::setToken(string t) {token = t;}
string SocketConnection::getUserName() {return userName;}
void SocketConnection::setUserName(string newUserName) {userName = newUserName;}
string SocketConnection::getPassword() {return password;}
void SocketConnection::setPassword(string newPassword) {password = newPassword;}
string SocketConnection::getServerIPAddress(){return inet_ntoa(servaddr.sin_addr);}
int SocketConnection::getServerPortNumber() {return servaddr.sin_port;}
string SocketConnection::getClientIPAddress(){return inet_ntoa(cliaddr.sin_addr);}
int SocketConnection::getClientPortNumber() {return cliaddr.sin_port;}



