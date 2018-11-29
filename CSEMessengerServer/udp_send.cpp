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
#include "SocketConnection.h"

using namespace std;

int main() 
{
	int ret;
    char send_buffer[1024];
	char recv_buffer[1024];
	socklen_t len;
	
	fd_set read_set;
	FD_ZERO(&read_set);
	
	struct timeval timeout;
	    timeout.tv_sec = 0;
		timeout.tv_usec = 100000; // 100 ms
	
	FD_ZERO( &read_set);
	
	SocketConnection* socketConnection = new SocketConnection("192.168.0.8", 31000, "192.168.0.12", 32000);
	
    while (1) 
	{
		fflush(stdout);
		FD_SET(fileno(stdin), &read_set);
		FD_SET(socketConnection->getSockfd_Server(), &read_set);
		
		select(socketConnection->getMaxfd(), &read_set, NULL, NULL, &timeout);
		
		if (FD_ISSET(fileno(stdin), &read_set)) 
		{
			string input;
			getline(cin, input);
			if ( input == "exit")
				break;
			
			if(input == "login")
			{
				string userName, password;
				cout << "Enter Username: "; getline(cin, userName);
				cout << "Enter Password: "; getline(cin, password);
				socketConnection->logIn(userName, password);
			}
			if (input == "message")
			{
				string destClientID, message;
				cout << "Enter destination username: "; getline(cin, destClientID);
				cout << "Enter message to send to destination user: "; getline(cin, message);
				socketConnection->sendMessage(destClientID, message);
				
			}
			
		}
		
		if (FD_ISSET(socketConnection->getSockfd_Server(), &read_set)) 
		{
			ret = recv(socketConnection->getSockfd_Server(), recv_buffer, sizeof(recv_buffer), 0);
			if (ret < 0) 
			{
				printf("recv() error: %s.\n", strerror(errno));
				break;
			}
			cout << recv_buffer << endl;
		}

		
    }
	

    return 0;
}

