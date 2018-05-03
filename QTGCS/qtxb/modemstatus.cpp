#include "modemstatus.h"
#include "digimeshpacket.h"
#include <QDebug>

ModemStatus::ModemStatus(QObject *parent) :
    DigiMeshPacket(parent)
{
}
void ModemStatus::readPacket(QByteArray rx){
    packet.clear();
    packet.append(rx);
    setStartDelimiter(rx.at(0));
    setLength(rx.at(2));
    if(rx.size() == rx.at(2)+4){
        setFrameType(rx.at(3));
        setStatus(rx.at(4));
        setChecksum(rx.at(5));
    }else{

        qDebug()<< "Invalid Packet Received!";
        qDebug()<< packet.toHex();
        packet.clear();
    }
}
void ModemStatus::setStatus(unsigned s){
    status = s;
}
unsigned ModemStatus::getStatus(){
    return status;
}
