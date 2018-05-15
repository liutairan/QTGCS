#ifndef REMOTECONTROL_XBEE_API_H
#define REMOTECONTROL_XBEE_API_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

#include "multiwii.h"

class RemoteControl_XBEE_API : public QObject
{
    Q_OBJECT
public:
    explicit RemoteControl_XBEE_API(QObject *parent = nullptr);
    RemoteControl_XBEE_API(QSerialPort *ser);
    ~RemoteControl_XBEE_API();

    void send(QByteArray data);
    void sendCMD(int cmd, Msp_rc_channels raw_rc);

signals:

public slots:

private:
    QSerialPort *serial;
    bool xbee_api_portFound;
};

#endif // REMOTECONTROL_XBEE_API_H
