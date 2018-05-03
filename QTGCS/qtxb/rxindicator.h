#ifndef RXINDICATOR_H
#define RXINDICATOR_H

#include "digimeshpacket.h"
#include <QByteArray>

class RXIndicator : public DigiMeshPacket
{
    QByteArray srcAddr64;
    QByteArray srcAddr16;
    unsigned receiveOptions;
    QByteArray data;

public:
    explicit RXIndicator(QObject *parent);
    void setSrcAddr64(QByteArray sa64);
    void setSrcAddr16(QByteArray sa16);
    void setReceiveOptions(unsigned ro);
    void setData(QByteArray d);
    QByteArray getSrcAddr64();
    QByteArray getSrcAddr16();
    unsigned getReceiveOptions();
    QByteArray getData();
    void readPacket(QByteArray rx);

};

#endif // RXINDICATOR_H
