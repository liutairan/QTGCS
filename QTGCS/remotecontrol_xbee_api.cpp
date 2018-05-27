#include "remotecontrol_xbee_api.h"

RemoteControl_XBEE_API::RemoteControl_XBEE_API(QObject *parent) : QObject(parent)
{

}

RemoteControl_XBEE_API::RemoteControl_XBEE_API(QSerialPort *ser)
{
    xbee_api_portFound = false;
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
                qDebug() << "XBEE API: Connected successfully";
                qDebug() << "XBEE API: Serial Port Name: " << serial->portName();
                xbee_api_portFound = true;
            }
        }
    }
    else
    {
        qDebug() << "XBEE API: Serial Port could not be opened";
    }
}

RemoteControl_XBEE_API::~RemoteControl_XBEE_API()
{
    if(serial->isOpen())
    {
        serial->close();
        qDebug() << "XBEE API: Serial Port closed successfully";
    }
}

void RemoteControl_XBEE_API::sendCMD(int cmd, Msp_rc_channels raw_rc)  // send rc values
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

void RemoteControl_XBEE_API::send(QByteArray data)
{
    if(xbee_api_portFound && serial->isOpen())
    {
        //qDebug() << "Send: " << data.toHex();
        serial->write(data);
        serial->flush();
    }
    else
    {
        qDebug() << "XBEE API: Cannot write to Serial Port - closed";
    }
}
