// By: Milton Young

#ifndef MYOBJECT_H
#define MYOBJECT_H

//--------------------------------------Imports----------------------------------------------//
#include <QObject>
#include "SocketConnection.h"
#include <queue>
//------------------------------------End of Imports-----------------------------------------//


// Function prototype
void* startEventLoop(void* arg);


/* Class Overview
    This class is just a wrapper for SocketConnection. This wrapper is used to
    connect the SocketConnection class with the QML side (GUI). This class
    has been converted into a QObject. It will be exposed to the QML side
    of this program.
*/
class MyObject: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString response READ getResponse NOTIFY newResponseReceived)
public:
    MyObject();
    SocketConnection* getSocketCommunicationObj();
    void emitSignal(QString response);

// These slots are callable from QML.
public slots:
    void initializeSocketConnection(QString clientIPAddress, QString clientPortNumber, QString serverIPAddress, QString serverPortNumber);
    void logIn(QString userName, QString password);
    void logOut(QString userName);
    void sendMessage(QString destClientID, QString message);
    QString getResponse();

// Signals can be retrieved in the QML side of this program.
signals:
    void newResponseReceived(QString response);


private:
    SocketConnection* socketConnection;
    QString response;


};
// Stucture
struct threadInfo{
    SocketConnection* sockConn;
    MyObject* myObj;
};
typedef threadInfo threadInfo_t;

#endif // MYOBJECT_H
