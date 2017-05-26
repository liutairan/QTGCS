#include "rxindicator.h"
#include "digimeshpacket.h"
#include <QDebug>

RXIndicator::RXIndicator(QObject *parent) :
    DigiMeshPacket(parent)
{
}
void RXIndicator::setSrcAddr64(QByteArray sa64){
    srcAddr64.clear();
    srcAddr64.append(sa64);
}
void RXIndicator::setSrcAddr16(QByteArray sa16){
    srcAddr16.clear();
    srcAddr16.append(sa16);
}
void RXIndicator::setData(QByteArray d){
    data.clear();
    data.append(d);
}
QByteArray RXIndicator::getSrcAddr64(){
    return srcAddr64;
}
QByteArray RXIndicator::getSrcAddr16(){
    return srcAddr16;
}
void RXIndicator::setReceiveOptions(unsigned ro){
    receiveOptions = ro;
}
unsigned RXIndicator::getReceiveOptions(){
    return receiveOptions;
}
QByteArray RXIndicator::getData(){
    return data;
}
void RXIndicator::readPacket(QByteArray rx){

    packet.clear();
    packet.append(rx);
    setStartDelimiter(rx.at(0));
    setLength(rx.at(2));
    if(rx.size() == rx.at(2)+4 && rx.size() > 15){
        setFrameType(rx.at(3));
        srcAddr64.append(rx.at(4));
        srcAddr64.append(rx.at(5));
        srcAddr64.append(rx.at(6));
        srcAddr64.append(rx.at(7));
        srcAddr64.append(rx.at(8));
        srcAddr64.append(rx.at(9));
        srcAddr64.append(rx.at(10));
        srcAddr64.append(rx.at(11));
        srcAddr16.append(rx.at(12));
        srcAddr16.append(rx.at(13));
        setReceiveOptions(rx.at(14));
        int count = 15;
        while(count < rx.size()-1){
            data.append(rx.at(count));
            count++;
        }
        setChecksum(rx.at(count));
    }else{

        qDebug()<< "Invalid Packet Received!";
        qDebug()<< packet.toHex();
        packet.clear();
    }
}
