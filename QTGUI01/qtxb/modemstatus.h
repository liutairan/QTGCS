#ifndef MODEMSTATUS_H
#define MODEMSTATUS_H

#include "digimeshpacket.h"
#include <QByteArray>

class ModemStatus : public DigiMeshPacket
{

    unsigned status;
public:
    explicit ModemStatus(QObject *parent);
    void readPacket(QByteArray rx);
    void setStatus(unsigned s);
    unsigned getStatus();
    
};

#endif // MODEMSTATUS_H
