#include "atcommand.h"
#include "digimeshpacket.h"

ATCommand::ATCommand(QObject *parent) :
    DigiMeshPacket(parent)
{
    setFrameType(0x08);
    setFrameId(0x00);
}
void ATCommand::setATCommand(QString command){
    atCommand.clear();
    atCommand.append(command.at(0));
    atCommand.append(command.at(1));
}
void ATCommand::setParameter(QByteArray array){
    parameter.clear();
    parameter.append(array);
}
QByteArray ATCommand::getATCommand(){
    return atCommand;
}
QByteArray ATCommand::getParameter(){
    return parameter;
}
void ATCommand::assemblePacket(){
    packet.clear();
    packet.append(getFrameType());
    packet.append(getFrameId());
    packet.append(getATCommand());
    packet.append(getParameter());
    setLength(packet.size());
    createChecksum(packet);
    packet.append(getChecksum());
    packet.insert(0, getStartDelimiter());
    packet.insert(1, getLength());
}

