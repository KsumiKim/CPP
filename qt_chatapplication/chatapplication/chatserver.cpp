#include "chatserver.h"
#include <QRegExp>

chatServer::chatServer(QObject *parent) : QTcpServer(parent)
{

    output.open("log.txt");
}

chatServer::~chatServer()
{
    output.close();
}

void chatServer::incomingConnection(int socketfd)
{
    QTcpSocket *client = new QTcpSocket(this);
    client->setSocketDescriptor(socketfd);
    clients.insert(client);

    qDebug() << "New client from:" << client->peerAddress().toString();
    output << "New client from:" << client->peerAddress().toString().toUtf8().constData() <<std::endl; //save log

    connect(client, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(client, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

void chatServer::readyRead()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    while(client->canReadLine())
    {
        QString line = QString::fromUtf8(client->readLine()).trimmed();
        qDebug() << "Read line:" << line;

        QRegExp meRegex("^/me:(.*)$");

        if(meRegex.indexIn(line) != -1)
        {
            QString user = meRegex.cap(1);
            users[client] = user;
            foreach(QTcpSocket *client, clients){
                client->write(QString("Server:" + user + " has joined.\n").toUtf8());
                output << QString("Server:" + user + " has joined.").toUtf8().constData() <<std::endl; //save log
            }
            sendUserList();
        }
        else if(users.contains(client))
        {
            QString message = line;
            QString user = users[client];
            qDebug() << "User:" << user;
            qDebug() << "Message:" << message;

            foreach(QTcpSocket *otherClient, clients){
                otherClient->write(QString(user + ":" + message + "\n").toUtf8());
                output << QString(user + ":" + message).toUtf8().constData() <<std::endl;
            }
        }
        else
        {
            qWarning() << "Got bad message from client:" << client->peerAddress().toString() << line;
        }
    }
}

void chatServer::disconnected()
{
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    qDebug() << "Client disconnected:" << client->peerAddress().toString();

    clients.remove(client);

    QString user = users[client];
    users.remove(client);

    sendUserList();
    foreach(QTcpSocket *client, clients){
        client->write(QString("Server:" + user + " has left.\n").toUtf8());
        output << QString("Server:" + user + " has left.").toUtf8().constData() <<std::endl;
    }
}

void chatServer::sendUserList()
{
    QStringList userList;
    foreach(QString user, users.values())
        userList << user;

    foreach(QTcpSocket *client, clients)
        client->write(QString("/users:" + userList.join(",") + "\n").toUtf8());
}
