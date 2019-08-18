#ifndef SERIALCOMMUNICATION_XBEE_AT_H
#define SERIALCOMMUNICATION_XBEE_AT_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QThread>

#include "multiwii.h"
#include "logmessage.h"
#include "msp_v1.h"
#include "msp_v2.h"

class SerialCommunication_XBEE_AT : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication_XBEE_AT(QObject *parent = nullptr);
    SerialCommunication_XBEE_AT(QSerialPort *ser, QList<QuadStates *> tempObjList);
    ~SerialCommunication_XBEE_AT();

    void send(QByteArray data);
    void sendCMD(int cmd);
    void sendCMD(int cmd, Msp_rc_channels raw_rc);
    void sendCMD(int cmd, int ind);
    void sendCMD(int cmd, Mission tempMission);

    void parseSensorStatus(QuadStates *);
    void parseArmingFlags(QuadStates *);
    void parseFlightModeFlags(QuadStates *);

    void downloadMission(int id, QuadStates *tempObj);
    void downloadMissions();
    void uploadMission(Mission tempMission, QuadStates *tempObj);
    void uploadMissions();
    bool checkMissionUpload(Mission mi_send, Mission mi_rec);

    void radioControl(int radioMode);
    void arm(QuadStates *);
    void disarm(QuadStates *);
    void nav(QuadStates *);
    void disnav(QuadStates *);
public slots:
    void readData();
    void missionDownloadedFlag();
    void missionUploadedFlag();
private:
    QSerialPort *serial;
    bool xbee_at_portFound;
    QList<QuadStates *> qsList;
    QByteArray buffer;
    void processPacket(QByteArray packet);
    bool missionDownloadFlag;
    bool missionUploadFlag;
    MSP_V1 *mspHandle1;
    MSP_V2 *mspHandle2;
signals:
    void qsReady(QList<QuadStates *> *);
    void missionDownloaded();
    void missionUploaded();
    void logMessageRequest(LogMessage);
};

#endif // SERIALCOMMUNICATION_XBEE_AT_H
