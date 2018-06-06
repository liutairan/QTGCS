#ifndef SERIALCOMMUNICATION_XBEE_API_H
#define SERIALCOMMUNICATION_XBEE_API_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "multiwii.h"
#include "logmessage.h"
#include "msp_v1.h"
#include "msp_v2.h"

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

class SerialCommunication_XBEE_API : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication_XBEE_API(QObject *parent = 0);
    SerialCommunication_XBEE_API(QSerialPort *ser, QList<QuadStates *> tempObjList);
    ~SerialCommunication_XBEE_API();

    void send(int objInd, QByteArray data);
    void sendCMD(int objInd, int cmd);
    void sendCMD(int objInd, int cmd, Msp_rc_channels raw_rc);
    void sendCMD(int objInd, int cmd, int ind);
    void sendCMD(int objInd, int cmd, Mission tempMission);

    void parseSensorStatus(QuadStates *);
    void parseArmingFlags(QuadStates *);
    void parseFlightModeFlags(QuadStates *);

    void downloadMission(int objInd, int id, QuadStates *tempObj);
    void downloadMissions(int objInd);
    void uploadMission(int objInd, Mission tempMission, QuadStates *tempObj);
    void uploadMissions(int objInd);
    bool checkMissionUpload(Mission mi_send, Mission mi_rec);

    void radioControl(int radioMode);
    void arm(QuadStates *);
    void disarm(QuadStates *);
    void nav(QuadStates *);
    void disnav(QuadStates *);
public slots:
    void readData(RXIndicator *response);
    //void missionDownloadedFlag(int objInd);
    void missionDownloadedFlag(int objInd, int missionId);
    void missionUploadedFlag(int objInd);
    void boxidsDownloadedFlag(int);

private:
    QSerialPort *serial;
    QTXB *xb;
    bool xbee_api_portFound;
    QList<QuadStates *> qsList;
    QByteArray buffer;
    void processPacket(int ind, QByteArray packet);
    //bool missionDownloadFlag[3];
    bool missionDownloadFlag[3][100];
    bool missionUploadFlag[3];
    bool boxidsDownloadFlag[3];
    MSP_V1 *mspHandle1;
    MSP_V2 *mspHandle2;

signals:
    void qsReady(QList<QuadStates *> *);
    //void missionDownloaded(int);
    void missionDownloaded(int, int);
    void missionUploaded(int);
    void boxidsDownloaded(int);

};

#endif // SERIALCOMMUNICATION_XBEE_API_H
