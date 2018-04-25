#include "transmitstatus.h"
#include "digimeshpacket.h"
#include <QDebug>

TransmitStatus::TransmitStatus(QObject *parent) :
    DigiMeshPacket(parent)
{
}
void TransmitStatus::readPacket(QByteArray rx){
    packet.clear();
    packet.append(rx);
    setStartDelimiter(rx.at(0));
    setLength(rx.at(2));
    if(rx.size() == rx.at(2)+4){
        setFrameType(rx.at(3));
        setFrameId(rx.at(4));
        reserved.append(rx.at(5));
        reserved.append(rx.at(6));
        setTransmitRetryCount(rx.at(7));
        setDeliveryStatus(rx.at(8));
        setDiscoveryStatus(rx.at(9));
        setChecksum(rx.at(10));
    }else{

        qDebug()<< "Invalid Packet Received!";
        qDebug()<< packet.toHex();
        packet.clear();
    }
}
void TransmitStatus:: setDeliveryStatus(unsigned ds){
    deliveryStatus = ds;
}
void TransmitStatus:: setTransmitRetryCount(unsigned trc){
    transmitRetryCount = trc;
}
void TransmitStatus:: setDiscoveryStatus(unsigned ds){
    discoveryStatus = ds;
}
unsigned TransmitStatus:: getDeliveryStatus(){
    return deliveryStatus;
}
unsigned TransmitStatus:: getTransmitRetryCount(){
    return transmitRetryCount;
}
unsigned TransmitStatus:: getDiscoveryStatus(){
    return discoveryStatus;
}
QByteArray TransmitStatus:: getReserved(){
    return reserved;
}
