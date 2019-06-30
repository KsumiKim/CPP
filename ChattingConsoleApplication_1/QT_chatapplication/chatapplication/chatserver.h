#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QStringList>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <QSet>
#include <QWidget>
#include <iostream>
#include <fstream>



class chatServer : public QTcpServer
{
    Q_OBJECT
public:
    chatServer(QObject *parent = nullptr);
    ~chatServer();

private slots:
    void readyRead();
    void disconnected();
    void sendUserList();

protected:
    void incomingConnection(int socketfd);

private:
    QSet<QTcpSocket*> clients;
    QMap<QTcpSocket*,QString> users;
    std::ofstream output;

};

#endif // CHATSERVER_H
