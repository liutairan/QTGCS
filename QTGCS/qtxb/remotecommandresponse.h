#ifndef REMOTECOMMANDRESPONSE_H
#define REMOTECOMMANDRESPONSE_H
#include "digimeshpacket.h"

#include <QObject>
#include <QByteArray>

class RemoteCommandResponse : public DigiMeshPacket
{

public:
    explicit RemoteCommandResponse(QObject *parent = 0);
    void readPacket(QByteArray rx);
    
signals:
    
public slots:
    
};

#endif // REMOTECOMMANDRESPONSE_H
