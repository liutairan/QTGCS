#include "remotecommandresponse.h"
#include "digimeshpacket.h"

RemoteCommandResponse::RemoteCommandResponse(QObject *parent) :
    DigiMeshPacket(parent)
{
}
void RemoteCommandResponse::readPacket(QByteArray rx){

}
