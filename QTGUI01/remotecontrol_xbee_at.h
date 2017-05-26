#ifndef REMOTECONTROL_XBEE_AT_H
#define REMOTECONTROL_XBEE_AT_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

#include "multiwii.h"

class RemoteControl_XBEE_AT : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl_XBEE_AT(QObject *parent = 0);
    RemoteControl_XBEE_AT(QSerialPort *ser);
    ~RemoteControl_XBEE_AT();

    void send(QByteArray data);
    void sendCMD(int cmd, Msp_rc_channels raw_rc);

private:
    QSerialPort *serial;
    bool xbee_at_portFound;
};

#endif // REMOTECONTROL_XBEE_AT_H
