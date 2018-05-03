#include "atcommandresponse.h"
#include "atcommand.h"
#include <QDebug>

ATCommandResponse::ATCommandResponse(QObject *parent) :
    DigiMeshPacket(parent)
{
}
void ATCommandResponse::readPacket(QByteArray rx){

    packet.clear();
    packet.append(rx);
    setStartDelimiter(rx.at(0));
    setLength(rx.at(2));
    if(rx.size() == rx.at(2)+4){
        setFrameType(rx.at(3));
        setFrameId(rx.at(4));
        setATCommand1(rx.at(5));
        setATCommand2(rx.at(6));
        setCommandStatus(rx.at(7));
        int count = 8;
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
void ATCommandResponse::setATCommand1(unsigned atc1){
    atCommand1 = atc1;
}
void ATCommandResponse::setATCommand2(unsigned atc2){
    atCommand2 = atc2;
}
void ATCommandResponse::setCommandStatus(unsigned cs){
    commandStatus = cs;
}
unsigned ATCommandResponse::getATCommand1(){
    return atCommand1;
}
unsigned ATCommandResponse::getATCommand2(){
    return atCommand2;
}
unsigned ATCommandResponse::getCommandStatus(){
    return commandStatus;
}

