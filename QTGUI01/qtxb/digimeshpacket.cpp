#include "digimeshpacket.h"

DigiMeshPacket::DigiMeshPacket(QObject *parent) :
    QObject(parent)

{
    startDelimiter = 0x7E;
}
void DigiMeshPacket::setStartDelimiter(unsigned sd){
     startDelimiter = sd;
}
void DigiMeshPacket::setLength(unsigned l){
    length = l;
}
void DigiMeshPacket::setFrameType(unsigned type){
    frameType = type;
}
void DigiMeshPacket::setFrameId(unsigned id){
    frameId = id;
}
void DigiMeshPacket::setChecksum(unsigned cs){
    checksum = cs;
}
unsigned DigiMeshPacket::getStartDelimiter(){
    return startDelimiter;
}
QByteArray DigiMeshPacket::getLength(){
    unsigned zero = 0x00;
    QByteArray l;
    l.append(zero);
    l.append(length);
    return l;
}
unsigned DigiMeshPacket::getFrameType(){
    return frameType;
}
unsigned DigiMeshPacket::getFrameId(){
    return frameId;
}
unsigned DigiMeshPacket::getChecksum(){
    return checksum;
}
void DigiMeshPacket::createChecksum(QByteArray array){
    int len = array.size();
    unsigned sum = 0x00;
    unsigned ff = 0xFF;
    unsigned fe = 0xFE;
    for(int i=0;i<len;i++)
    {
       unsigned a = array.at(i);
        if (a == 4294967295){
            a = ff;
        } else if (a == 4294967294){
            a = fe;
        }

        sum += a;
    }
    setChecksum((ff - sum) & 0x000000FF);
}
QByteArray DigiMeshPacket::getPacket(){
    return packet;
}
