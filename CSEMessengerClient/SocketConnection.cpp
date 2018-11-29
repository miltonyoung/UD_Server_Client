#include "SocketConnection.h"
using namespace std;

// Socket Message Examples

// Authentication Successful Example:   "server->client_a#[Authentication-Successful]<ABCDEF>"
// Authentication Failed Example:       "server->client_a#[Authentication-Failed]<000000>"
// Message Successful Example:          "server->client_a#[Message-Successful]<ABCDEF><1234567890>some_message"
// Message Failed Example:              "server->client_a#[Message-Failed]<ABCDEF><1234567890>Error: token error!"
// Message Example:                     "client_a->client_b#[Message]<token><msg_id>msg"


// The transParams const variable is used to process
// the commands as they are received.
const transmissionParams_t transParams[] = {
                                                {"Authentication-Successful", 	1, "%[^-]->%[^#]#[%[^]]]<%[^>]>"                },
                                                {"Authentication-Failed",       1, "%[^-]->%[^#]#[%[^]]]<%[^>]>"                },
                                                {"Message-Successful",          2, "%[^-]->%[^#]#[%[^]]]<%[^>]><%[^>]>%[^\n]"   },
                                                {"Message-Failed",              2, "%[^-]->%[^#]#[%[^]]]<%[^>]><%[^>]>%[^\n]"	},
                                                {"Message",                     2, "%[^-]->%[^#]#[%[^]]]<%[^>]><%[^>]>%[^\n]"	},
                                                {"",                            0, ""                                           }
                                            };

// Help Function Prototypes
transmissionParams_t getTransParams(string type);
int charCount(string str, char ch);
string generateNewMsgID();

// Default Constructor
SocketConnection::SocketConnection()
{
    sockfd_server = -1;
    sockfd_client = -1;
    maxfd = -1;
    token = "000000";
    userName = "";
    password = "";
}

// Overloaded Constructor
SocketConnection::SocketConnection(string clientIPAddress, int clientPortNumber, string serverIPAddress, int serverPortNumber)
{
    int ret;
	WSADATA wsa;

	//Initialise winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
        printf("Failed. Error Code : %d", WSAGetLastError());
		return;
	}

	//create socket
	if ((sockfd_server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
	{
        printf("socket() failed with error code : %d", WSAGetLastError());
		return;
	}

    if ((sockfd_client = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR)
    {
        printf("socket() failed with error code : %d", WSAGetLastError());
        return;
    }

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

    token = "000000";
	maxfd = sockfd_server + 1;
}

// Function: sends login request to server
int SocketConnection::logIn(string userName, string password)
{
	this->userName = userName;
	this->password = password;
	stringstream acc;
	
    acc << userName << "->server#[Authenticate]<" << password << "><" << getClientIPAddress() << "><" << getClientPortNumber() << ">";

	if (send(sockfd_server, acc.str().c_str(), acc.str().length() + 1, 0) == SOCKET_ERROR)
	{
        printf("sendto() failed with error code : %d", WSAGetLastError());
		return 0;
	}
	return 1;
}

// Fucnction: Sends message to server with end user spcified
int SocketConnection::sendMessage(string destClientID, string message)
{
	char responseArray[BUFLEN];
    memset(responseArray, 0, sizeof(responseArray)); // Initialize buffer indexes to 0;
    sprintf(responseArray, "%s->%s#[Message]<%s><%s>%s", userName.c_str(), destClientID.c_str(), token.c_str(), generateNewMsgID().c_str() ,message.c_str());
	if (send(sockfd_server, responseArray, strlen(responseArray), 0) == SOCKET_ERROR)
	{
        printf("sendto() failed with error code : %d", WSAGetLastError());
		return 0;
	}
	return 1;
}

// Function: Handler that sends message received acknowledgement to server
int SocketConnection::recvMessageHandle(transcribedMessage_t transMsg)
{
    //int ret;
    char responseArray[BUFLEN];
    memset(responseArray, 0, sizeof(responseArray));
    sprintf(responseArray, "%s->server#[Message-Successful]<%s><%s>Received: %s", userName.c_str(), token.c_str(), transMsg.msgID.c_str(), transMsg.msg.c_str());
    if (send(sockfd_server, responseArray, strlen(responseArray), 0) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d", WSAGetLastError());
        return 0;
    }
    return 1;
}

// Function: sends logOut request to server
int SocketConnection::logOut(string destClientID)
{
    //int ret;
    stringstream acc;

    acc << destClientID << "->server#[Logout]<" << token << ">";

    if (send(sockfd_server, acc.str().c_str(), acc.str().length() + 1, 0) == SOCKET_ERROR)
    {
        printf("sendto() failed with error code : %d", WSAGetLastError());
        return 0;
    }
    return 1;

}

// Function: processes incoming transmissions from server and outputs
// a struct that holds each part of the transmission in their respective variables.
transcribedMessage_t processIncomingTransmission(char recv_buffer[1024])
{
    transcribedMessage_t transMsg;
    string recv_buffer_Str(recv_buffer); // char* to string to user string functions

    //-------------------------------------------Transmission Format Checker and Transmission Type Parsing------------------------------------//

    /* Parsing out the message type from the transmission.
       If the message type is not in correct format, an empty
       transcribedMessage_t will be returned to function caller.
    */
    int transmissionTypeSize = -1;
    if(recv_buffer_Str.find("[", 0) != -1 &&  recv_buffer_Str.find("]", 0))
        transmissionTypeSize = recv_buffer_Str.find("]", 0) - recv_buffer_Str.find("[", 0);
    else
        return transMsg;

    string transmissionType = recv_buffer_Str.substr(recv_buffer_Str.find("[", 0) + 1, transmissionTypeSize - 1);

    /* Checking transmission for correct format.
       If the message type is not in correct format, an empty
       transcribedMessage_t will be returned to function caller.
    */
    int indexOfArrow = recv_buffer_Str.find("->", 0);
    int indexOfHashTag = recv_buffer_Str.find("#", 0);
    transmissionParams_t t = getTransParams(transmissionType);
    if(!(charCount(recv_buffer_Str, '<') == charCount(recv_buffer_Str.substr(indexOfHashTag), '>') && charCount(recv_buffer_Str, '<') == t.paramCount ))
        return transMsg;

   //-------------------------------------End of Transmission Format Checker and Transmission Type Parsing------------------------------------//


    //-------------------------------------------Parsing the Rest of the Transmission---------------------------------------------------------//

    char src[1000]; char dest[1000]; char msgType[1000];
    char token[1000]; char msgID[1000]; char msg[1000];


    // Authentication Successful Example:   "server->client_a#[Authentication-Successful]<ABCDEF>"
    if(t.transmissionType == "Authentication-Successful")
    {
        int parseValue = sscanf(recv_buffer_Str.c_str(), t.format.c_str(), src, dest, msgType, token);
        if( parseValue < 1)
        {
            return transMsg;
        }
    }

    // Authentication Failed Example:       "server->client_a#[Authentication-Failed]<000000>"
    else if(t.transmissionType == "Authentication-Failed")
    {
        if(sscanf(recv_buffer_Str.c_str(), t.format.c_str(), src, dest, msgType, token) < 1)
            return transMsg;
    }
    // Message Successful Example:          "server->client_a#[Message-Successful]<ABCDEF><1234567890>some_message"
    else if(t.transmissionType == "Message-Successful")
    {
        if(sscanf(recv_buffer_Str.c_str(), t.format.c_str(), src, dest, msgType, token, msgID, msg) < 1)
            return transMsg;
    }
    // Message Failed Example:              "server->client_a#[Message-Failed]<ABCDEF><1234567890>Error: token error!"
    else if(t.transmissionType == "Message-Failed")
    {
        if(sscanf(recv_buffer_Str.c_str(), t.format.c_str(), src, dest, msgType, token, msgID, msg) < 1)
            return transMsg;
    }
    // Message Example:                     "client_a->client_b#[Message]<token><msg_id>msg"
    else if(t.transmissionType == "Message")
    {
        if(sscanf(recv_buffer_Str.c_str(), t.format.c_str(), src, dest, msgType, token, msgID, msg) < 1)
            return transMsg;
    }
    else
        return transMsg;
    //------------------------------------End of Parsing the Rest of the Transmission---------------------------------------------------------//


    // save data
    transMsg.src = string(src); transMsg.dest = string(dest); transMsg.msgType = string(msgType);
    transMsg.token = string(token);
    transMsg.msgID = string(msgID); transMsg.msg = string(msg);

    return transMsg;

}


// Mutators and Accessor Functions
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

//---------------------------------------Helper Functions------------------------------------------//


transmissionParams_t getTransParams(string type)
{
    transmissionParams_t tran;
    for(int i = 0 ; i < (sizeof(transParams)/ sizeof(transmissionParams_t)); i++)
    {
        if(transParams[i].transmissionType == type)
            tran = transParams[i];
    }
    return tran;
}

int charCount(string str, char ch)
{
    int count = 0;
    for(int i = 0; i < str.length(); i++)
    {
        if(str[i] == ch)
            count++;
    }
    return count;
}

string generateNewMsgID()
{
    char arr[10];

    for(int i = 0; i < 10; i++)
    {
        int myInt = (rand() % 10);
        arr[i] = to_string(myInt)[0];
    }
    return string(arr).substr(0, 10);
}

//------------------------------------End of Helper Functions--------------------------------//

