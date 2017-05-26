#ifndef ATCOMMANDRESPONSE_H
#define ATCOMMANDRESPONSE_H

#include "atcommand.h"
#include <QByteArray>

class ATCommandResponse : public DigiMeshPacket
{
    unsigned atCommand1;
    unsigned atCommand2;
    unsigned commandStatus;
    QByteArray data;
public:
    explicit ATCommandResponse(QObject *parent);

    void setATCommand1(unsigned atc1);
    void setATCommand2(unsigned atc2);
    void setCommandStatus(unsigned cs);
    unsigned getATCommand1();
    unsigned getATCommand2();
    unsigned getCommandStatus();
    void readPacket(QByteArray rx);
    void setData(QByteArray d);
    QByteArray getData();

};

#endif // ATCOMMANDRESPONSE_H
