#ifndef REMOTECONTROL_XBEE_API_H
#define REMOTECONTROL_XBEE_API_H

#include <QObject>

class RemoteControl_XBEE_API : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl_XBEE_API(QObject *parent = nullptr);

signals:

public slots:
};

#endif // REMOTECONTROL_XBEE_API_H