#include "remotecontrol_xbee_at.h"

RemoteControl_XBEE_AT::RemoteControl_XBEE_AT(QObject *parent) : QObject(parent)
{

}

RemoteControl_XBEE_AT::RemoteControl_XBEE_AT(QSerialPort *ser)
{
    xbee_at_portFound = false;
    serial = ser;

    if (serial->open(QIODevice::ReadWrite))
    {
        if(serial->setBaudRate(QSerialPort::Baud115200) &&
                serial->setDataBits(QSerialPort::Data8) &&
                serial->setParity(QSerialPort::NoParity) &&
                serial->setStopBits(QSerialPort::OneStop) &&
                serial->setFlowControl(QSerialPort::NoFlowControl))
        {
            if(serial->isOpen())
            {
                qDebug() << "XBEE AT: Connected successfully";
                qDebug() << "XBEE AT: Serial Port Name: " << serial->portName();
                xbee_at_portFound = true;
            }
        }
    }
    else
    {
        qDebug() << "XBEE AT: Serial Port could not be opened";
    }
}

RemoteControl_XBEE_AT::~RemoteControl_XBEE_AT()
{
    if(serial->isOpen())
    {
        serial->close();
        qDebug() << "XBEE AT: Serial Port closed successfully";
    }
}

void RemoteControl_XBEE_AT::sendCMD(int cmd, Msp_rc_channels raw_rc)  // send rc values
{
    QByteArray output;
    //output.clear();
    char checksum = 0;
    output.append("$M<");
    output.append(char(0xFF & 16));
    output.append(char(0xFF & cmd));
    checksum = (char(0xFF & 16)) ^ (char(0xFF & cmd));
    for (int i = 0; i < 8; i++)
    {
        uint16_t tempValue = raw_rc.rcData[i];
        output.append(char(0xFF & tempValue));
        checksum = checksum ^ (char(0xFF & tempValue));
        output.append(char(0xFF & (tempValue >> 8)));
        checksum = checksum ^ (char(0xFF & (tempValue >> 8)));
    }
    // checksum byte
    output.append(checksum );
    //qDebug() << output.toHex();
    send(output);
}

void RemoteControl_XBEE_AT::send(QByteArray data)
{
    if(xbee_at_portFound && serial->isOpen())
    {
        //qDebug() << "Send: " << data.toHex();
        serial->write(data);
        serial->flush();
    }
    else
    {
        qDebug() << "XBEE AT: Cannot write to Serial Port - closed";
    }
}
