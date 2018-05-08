#ifndef SERIALCOMMUNICATION_H
#define SERIALCOMMUNICATION_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "serialcommunication_usb.h"
#include "serialcommunication_xbee_at.h"
#include "serialcommunication_xbee_api.h"

#include "multiwii.h"
#include "quadstates.h"
#include "logmessage.h"

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

class SerialCommunication : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication(QObject *parent = 0);
    SerialCommunication(QSerialPort *ser, QString connMethod, QString addrList[]);
    ~SerialCommunication();
    void send(TXRequest *request);
    void broadcast(QString data);
    void unicast(QByteArray address, QString data);
    void stopSerial();
    void createObjs();
    void PreLoadInfo();
    void PreCheck();
    void RegularLoadInfo();
    void RegularCheck();
    void RegularLoadOverview();
    void RegularLoadAllGPS();
    void RegularLoadQuad1();
    void RegularLoadQuad2();
    void RegularLoadQuad3();
    void RegularLoadInfoLoose();
    void UploadMissions(int objInd, Mission_list);
    void DownloadMissions(int objInd);
    void RegularArmAndNavAll(int radioMode);
    void RegularArmAll();
    void RegularDisarmAll();
    void RegularNavAll();
    void RegularDisnavAll();
    void AbortMission();
    SerialCommunication_USB *sc_usb;
    SerialCommunication_XBEE_AT *sc_xbee_at;
    SerialCommunication_XBEE_API *sc_xbee_api;
    QList<QuadStates *> quadstates_list;
    QString addressList[3];
    bool everRunFlag;
    QTime tReceive;
    //LogMessage tempLogMessage;
signals:
    void quadsStatesChangeRequest(QList<QuadStates *> *);  // update quads states
    void logMessageRequest(LogMessage);
private:
    QSerialPort *serial;
    QString connectionMethod;
public slots:
    void Update(QList<QuadStates *> *);
    void logMessage(LogMessage);
};

#endif // SERIALCOMMUNICATION_H
