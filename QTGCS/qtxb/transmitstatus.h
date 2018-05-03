#ifndef TRANSMITSTATUS_H
#define TRANSMITSTATUS_H
#include "digimeshpacket.h"
#include <QByteArray>

class TransmitStatus : public DigiMeshPacket
{
    QByteArray reserved;
    unsigned deliveryStatus;
    unsigned transmitRetryCount;
    unsigned discoveryStatus;
public:
    explicit TransmitStatus(QObject *parent);

     void readPacket(QByteArray rx);
     void setDeliveryStatus(unsigned ds);
     void setTransmitRetryCount(unsigned trc);
     void setDiscoveryStatus(unsigned ds);
     unsigned getDeliveryStatus();
     unsigned getTransmitRetryCount();
     unsigned getDiscoveryStatus();
     QByteArray getReserved();
    
};

#endif // TRANSMITSTATUS_H
