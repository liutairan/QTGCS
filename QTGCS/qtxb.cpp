#include "qtxb.h"

#include "qtxb/digimeshpacket.h"

#include "qtxb/atcommand.h"
#include "qtxb/atcommandqueueparam.h"
#include "qtxb/txrequest.h"
#include "qtxb/txrequestexplicit.h"
#include "qtxb/remotecommandrequest.h"

#include "qtxb/atcommandresponse.h"
#include "qtxb/modemstatus.h"
#include "qtxb/transmitstatus.h"
#include "qtxb/rxindicator.h"
#include "qtxb/rxindicatorexplicit.h"
#include "qtxb/nodeidentificationindicator.h"
#include "qtxb/remotecommandresponse.h"


#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QList>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QCoreApplication>

QTXB::QTXB(QObject *parent) :
    QObject(parent)
{
}
QTXB::QTXB(QSerialPort *ser){

    xbeeFound = false;
    serial = ser;

    connect(serial, SIGNAL(readyRead()), SLOT(readData()));

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
                qDebug() << "XBEE: Connected successfully";
                qDebug() << "XBEE: Serial Port Name: " << serial->portName();
                xbeeFound = true;
            }
        }
    }
    else
    {
        qDebug() << "XBEE: Serial Port could not be opened";
    }

}


QTXB::~QTXB()
{
    if(serial->isOpen())
    {
        serial->close();
        qDebug() << "XBEE: Serial Port closed successfully";
    }
}
void QTXB::displayATCommandResponse(ATCommandResponse *digiMeshPacket){
    qDebug() << "Received ATCommandResponse: " << digiMeshPacket->getPacket().toHex();
}
void QTXB::displayModemStatus(ModemStatus *digiMeshPacket){
    qDebug() << "Received ModemStatus: " << digiMeshPacket->getPacket().toHex();
}
void QTXB::displayTransmitStatus(TransmitStatus *digiMeshPacket){
    //qDebug() << "Received TransmitStatus: " << digiMeshPacket->getPacket().toHex();
}
void QTXB::displayRXIndicator(RXIndicator *digiMeshPacket){
    /*
    qDebug() << "Received RXIndicator: "
             << digiMeshPacket->getSrcAddr64().toHex()
             << digiMeshPacket->getSrcAddr16().toHex()
             << digiMeshPacket->getData().toHex();
             */
    //emit packetReady(digiMeshPacket->getData());
    emit packetReady(digiMeshPacket);
}
void QTXB::displayRXIndicatorExplicit(RXIndicatorExplicit *digiMeshPacket){
    qDebug() << "Received RXIndicatorExplicit: " << digiMeshPacket->getPacket().toHex();
}
void QTXB::displayNodeIdentificationIndicator(NodeIdentificationIndicator *digiMeshPacket){
    qDebug() << "Received NodeIdentificationIndicator: " << digiMeshPacket->getPacket().toHex();
}
void QTXB::displayRemoteCommandResponse(RemoteCommandResponse *digiMeshPacket){
    qDebug() << "Received RemoteCommandResponse: " << digiMeshPacket->getPacket().toHex();
}

void QTXB::send(TXRequest *request)
{
    request->assemblePacket();
    if(xbeeFound && serial->isOpen())
    {
        //qDebug() << "Transmit: " << request->getPacket().toHex();
        serial->write(request->getPacket());

        serial->flush();
    }
    else
    {
        qDebug() << "XBEE: Cannot write to Serial Port - closed";
    }
}

void QTXB::broadcast(QString data)
{
    TXRequest *request = new TXRequest(this);
    request->setData(data.toLatin1());
    send(request);
}
void QTXB::unicast(QByteArray address, QString data){
    TXRequest *request = new TXRequest(this);
    request->setDestAddr64(address);
    request->setData(data.toLatin1());
    send(request);
}

void QTXB::unicast(QByteArray address, QByteArray data){
    TXRequest *request = new TXRequest(this);
    request->setDestAddr64(address);
    request->setData(data);
    send(request);
}

void QTXB::readData()
{
    unsigned startDelimiter = 0x7E;

    buffer.append(serial->readAll());

    QByteArray packet;

    if (buffer.size() >= 1)
    {
        while((unsigned char)buffer.at(0) != (unsigned char)startDelimiter)
        {
            buffer.remove(0, 1);
            if (buffer.size() == 0)
            {
                break;
            }
        }
        if(buffer.size() > 2){
            unsigned length = buffer.at(2)+4;
            if((unsigned char)buffer.size() >= (unsigned char)length){
                packet.append(buffer.left(length));
                processPacket(packet);
                buffer.remove(0, length);
            }
        }
    }
}
void QTXB::processPacket(QByteArray packet){

    unsigned packetType = (unsigned char)packet.at(3);
    switch (packetType)
    {
    case pATCommandResponse:
    {
        ATCommandResponse *response = new ATCommandResponse(this);
        response->readPacket(packet);
        emit receivedATCommandResponse(response);
        break;
    }
    case pModemStatus:
    {
        ModemStatus *response = new ModemStatus(this);
        response->readPacket(packet);
        emit receivedModemStatus(response);
        break;
    }
    case pTransmitStatus:
    {
        TransmitStatus *response = new TransmitStatus(this);
        response->readPacket(packet);
        emit receivedTransmitStatus(response);
        break;
    }
    case pRXIndicator:
    {
        RXIndicator *response = new RXIndicator(this);
        response->readPacket(packet);
        emit receivedRXIndicator(response);
        break;
    }
    case pRXIndicatorExplicit:
    {
        RXIndicatorExplicit *response = new RXIndicatorExplicit(this);
        response->readPacket(packet);
        emit receivedRXIndicatorExplicit(response);
        break;
    }
    case pNodeIdentificationIndicator:
    {
        NodeIdentificationIndicator *response = new NodeIdentificationIndicator(this);
        response->readPacket(packet);
        emit receivedNodeIdentificationIndicator(response);
        break;
    }
    case pRemoteCommandResponse:
    {
        RemoteCommandResponse *response = new RemoteCommandResponse(this);
        response->readPacket(packet);
        emit receivedRemoteCommandResponse(response);
        break;
    }
    default:
        qDebug() << "Error:  Unknown Packet: " << packet.toHex();
    }
}
