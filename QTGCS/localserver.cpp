#include "localserver.h"

LocalServer::LocalServer(QObject *parent) :
    QObject(parent)
{
    server = new QTcpServer(this);
    connect(server, SIGNAL(newConnection()), this, SLOT(newConnection()));

    if (!server->listen(QHostAddress::Any, 8080))
    {
        //qDebug() << "Server could not start";
        // Send log info to main GUI
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Local Server");
        tempLogMessage.message = QString("Server could not start.");
        emit logMessageRequest(tempLogMessage);
        //
    }
    else
    {
        //qDebug() << "Server started";
        // Send log info to main GUI
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("Local Server");
        tempLogMessage.message = QString("Server started.");
        emit logMessageRequest(tempLogMessage);
        //
    }
}

LocalServer::~LocalServer()
{
    foreach (QTcpSocket *tempSocket, socketList) {
        tempSocket->close();
    }
    delete server;
}

void LocalServer::newConnection()
{
    QTcpSocket *tempSocket;
    tempSocket = server->nextPendingConnection();
    //qDebug() << tempSocket->localAddress() << tempSocket->localPort();
    // Send log info to main GUI
    LogMessage tempLogMessage;
    tempLogMessage.id = QString("Local Server");
    tempLogMessage.message = tempSocket->localAddress().toString() + " " + QString::number(tempSocket->localPort(), 10);
    emit logMessageRequest(tempLogMessage);
    //
    connect(tempSocket, SIGNAL(readyRead()), this, SLOT(readData()));
    // Clear old connections
    //    If not clear old connections, then old connection still
    //    stays in the socketList, then it will give errors.
    //    For example, if the connection comes from a cellphone.
    //    If the cellphone gets locked and unlocked,
    //    then the socket will not be able to receive the data
    //    from the app. Reconnect from the app will cause the problem.
    //    After clear the old connections, then append the new connection,
    //    this problem can be solved because it is connected with a new
    //    socket everytime.
    socketList.clear();
    socketList.append(tempSocket);
    //qDebug() << socketList;
}

void LocalServer::readData()
{
    QByteArray msg;
    foreach (QTcpSocket *tempSocket, socketList) {
        QByteArray singleMsg = tempSocket->readAll();
        if (singleMsg.length() > 0)
        {
            msg.append(singleMsg);
        }
    }
    parseMessage(msg);
    foreach (QTcpSocket *tempSocket, socketList) {
        tempSocket->write(msg);
        tempSocket->waitForBytesWritten(10);
    }
}

void LocalServer::parseMessage(QByteArray msg)
{
    QStringList msgList = QString::fromUtf8(msg).split(" ");

    foreach (QString singleMsg, msgList)
    {
        emit inputReceived(singleMsg);
    }

}
