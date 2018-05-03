#include "txrequest.h"
#include "digimeshpacket.h"

TXRequest::TXRequest(QObject *parent) :
    DigiMeshPacket(parent)
{
    unsigned zero = 0x00;
    unsigned oxff = 0xFF;
    unsigned oxfe = 0xFE;
    destAddr16.append(oxff);
    destAddr16.append(oxfe);
    destAddr64.append(zero);
    destAddr64.append(zero);
    destAddr64.append(zero);
    destAddr64.append(zero);
    destAddr64.append(zero);
    destAddr64.append(zero);
    destAddr64.append(oxff);
    destAddr64.append(oxff);

    broadcastRadius = 1;
    transmitOptions = 0x00;
    setFrameType(0x10);
    setFrameId(0x01);
}
void TXRequest::setBroadcastRadius(int rad){
    broadcastRadius = rad;
}
void TXRequest::setTransmitOptions(unsigned to){
    transmitOptions = to;
}
void TXRequest::setDestAddr64(QByteArray da64){
    destAddr64.clear();
    destAddr64.append(da64);
}
void TXRequest::setDestAddr16(QByteArray da16){
    destAddr16.clear();
    destAddr16.append(da16);
}
void TXRequest::setData(QByteArray d){
    data.clear();
    data.append(d);
}
QByteArray TXRequest::getDestAddr64(){
    return destAddr64;
}
QByteArray TXRequest::getDestAddr16(){
    return destAddr16;
}
unsigned TXRequest::getBroadcastRadius(){
    return broadcastRadius;
}
unsigned TXRequest::getTransmitOptions(){
    return transmitOptions;
}
QByteArray TXRequest::getData(){
    return data;
}
void TXRequest::assemblePacket(){
    packet.clear();
    packet.append(getFrameType());
    packet.append(getFrameId());
    packet.append(getDestAddr64());
    packet.append(getDestAddr16());
    packet.append(getBroadcastRadius());
    packet.append(getTransmitOptions());
    packet.append(getData());
    setLength(packet.size());
    createChecksum(packet);
    packet.append(getChecksum());
    packet.insert(0, getStartDelimiter());
    packet.insert(1, getLength());

}

