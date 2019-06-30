#include "chatserver.h"
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    chatServer *server = new chatServer();
   // bool success = server->listen(QHostAddress::Any, 4300);
    bool success = server->listen(QHostAddress("10.10.14.99"),8888);
    if(!success)
    {
        qFatal("Could not listen on port 4200.");
    }

    qDebug() << "Ready";

    return app.exec();
}
