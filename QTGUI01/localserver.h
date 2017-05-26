#ifndef LOCALSERVER_H
#define LOCALSERVER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class LocalServer : public QObject
{
    Q_OBJECT
public:
    explicit LocalServer(QObject *parent = 0);
    ~LocalServer();

signals:

public slots:
    void newConnection();
    void readData();
    void parseMessage(QByteArray msg);

private:
    QTcpServer *server;
    //QTcpSocket *socket;
    QList<QTcpSocket *> socketList;
signals:
    void inputReceived(QString msg);
public slots:
};

#endif // LOCALSERVER_H
