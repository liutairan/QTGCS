#ifndef ATCOMMAND_H
#define ATCOMMAND_H

#include "digimeshpacket.h"
#include <QByteArray>

class ATCommand : public DigiMeshPacket
{
    QByteArray atCommand;
    QByteArray parameter;
public:
    explicit ATCommand(QObject *parent);
    void setATCommand(QString command);
    void setParameter(QByteArray array);
    QByteArray getATCommand();
    QByteArray getParameter();
    void assemblePacket();
      
};

#endif // ATCOMMANDPACKET_H
