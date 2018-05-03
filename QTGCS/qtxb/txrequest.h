#ifndef TXREQUEST_H
#define TXREQUEST_H

#include "digimeshpacket.h"


#include <QByteArray>

class TXRequest : public DigiMeshPacket
{
    QByteArray destAddr64;
    QByteArray destAddr16;
    unsigned broadcastRadius;
    unsigned transmitOptions;
    QByteArray data;
public:
    explicit TXRequest(QObject *parent);
    void setBroadcastRadius(int rad);
    void setTransmitOptions(unsigned to);
    void setDestAddr64(QByteArray da64);
    void setDestAddr16(QByteArray da16);
    void setData(QByteArray d);
    QByteArray getDestAddr64();
    QByteArray getDestAddr16();
    unsigned getBroadcastRadius();
    unsigned getTransmitOptions();
    QByteArray getData();
    void assemblePacket();




    
};

#endif // TXREQUEST_H
