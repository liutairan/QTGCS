#ifndef LOGMESSAGE_H
#define LOGMESSAGE_H

#include <QObject>
#include <QDebug>
#include <QString>

class LogMessage
{
public:
    LogMessage();
    QString id = "";
    QString message = "";
};

#endif // LOGMESSAGE_H
