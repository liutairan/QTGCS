#ifndef REMOTECOMMANDREQUEST_H
#define REMOTECOMMANDREQUEST_H

#include <QObject>

class RemoteCommandRequest : public QObject
{
    Q_OBJECT
public:
    explicit RemoteCommandRequest(QObject *parent = 0);
    
signals:
    
public slots:
    
};

#endif // REMOTECOMMANDREQUEST_H
