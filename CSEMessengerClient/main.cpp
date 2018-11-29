#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "myobject.h"



int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // Declaring an instance of the custom QObject and exposing it to QML
    MyObject* myObject = new MyObject();
        engine.rootContext()->setContextProperty("myObject", myObject);

        // Starting socket monitoring thread
        threadInfo_t t;
        t.sockConn = myObject->getSocketCommunicationObj();
        t.myObj = myObject;
        pthread_t newThread;
        pthread_create(&newThread, NULL, startEventLoop, (void *)(&t));



    return app.exec();
}
