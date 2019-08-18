#ifndef QTXB_H
#define QTXB_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
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

class QTXB : public QObject
{
    Q_OBJECT
public:
    explicit QTXB(QObject *parent = nullptr);
    QTXB(QSerialPort *ser);
    void send(TXRequest *request);
    void broadcast(QString data);
    void unicast(QByteArray address, QString data);
    void unicast(QByteArray address, QByteArray data);
    ~QTXB();
    static const unsigned pATCommand = 0x08;
    static const unsigned  pATCommandQueue = 0x09;
    static const unsigned  pTXRequest = 0x10;
    static const unsigned  pTXRequestExplicit = 0x11;
    static const unsigned  pRemoteCommandRequest = 0x17;

    static const unsigned  pATCommandResponse = 0x88;
    static const unsigned  pModemStatus = 0x8A;
    static const unsigned  pTransmitStatus = 0x8B;
    static const unsigned  pRXIndicator = 0x90;
    static const unsigned  pRXIndicatorExplicit = 0x91;
    static const unsigned  pNodeIdentificationIndicator = 0x95;
    static const unsigned  pRemoteCommandResponse = 0x97;

signals:
    void receivedATCommandResponse(ATCommandResponse *response);
    void receivedModemStatus(ModemStatus *response);
    void receivedTransmitStatus(TransmitStatus *response);
    void receivedRXIndicator(RXIndicator *response);
    void receivedRXIndicatorExplicit(RXIndicatorExplicit *response);
    void receivedNodeIdentificationIndicator(NodeIdentificationIndicator *response);
    void receivedRemoteCommandResponse(RemoteCommandResponse *response);
    void packetReady(RXIndicator *response);
public slots:
    void readData();
    void displayATCommandResponse(ATCommandResponse *digiMeshPacket);
    void displayModemStatus(ModemStatus *digiMeshPacket);
    void displayTransmitStatus(TransmitStatus *digiMeshPacket);
    void displayRXIndicator(RXIndicator *digiMeshPacket);
    void displayRXIndicatorExplicit(RXIndicatorExplicit *digiMeshPacket);
    void displayNodeIdentificationIndicator(NodeIdentificationIndicator *digiMeshPacket);
    void displayRemoteCommandResponse(RemoteCommandResponse *digiMeshPacket);


private:
    QSerialPort *serial;
    bool xbeeFound;
    QByteArray buffer;
    void processPacket(QByteArray packet);
};

#endif
