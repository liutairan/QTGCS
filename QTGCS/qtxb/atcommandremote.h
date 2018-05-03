#ifndef ATCOMMANDREMOTE_H
#define ATCOMMANDREMOTE_H

#include "atcommand.h"
#include <QByteArray>

class ATCommandRemote : public ATCommand
{

public:
    explicit ATCommandRemote(QObject *parent);
    QByteArray remoteCommandOptions;

    
};

#endif // ATCOMMANDREMOTE_H
