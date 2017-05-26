#ifndef SERIALCOMMUNICATION_USB_H
#define SERIALCOMMUNICATION_USB_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QEventLoop>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

#include "multiwii.h"

class SerialCommunication_USB : public QObject
{
    Q_OBJECT
public:
    explicit SerialCommunication_USB(QObject *parent = 0);
    SerialCommunication_USB(QSerialPort *ser, QList<QuadStates *> tempObjList);
    ~SerialCommunication_USB();

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
    bool serialportFound;
    QList<QuadStates *> qsList;
    QByteArray buffer;
    void processPacket(QByteArray packet);
    bool missionDownloadFlag;
    bool missionUploadFlag;
signals:
    void qsReady(QList<QuadStates *> *);
    void missionDownloaded();
    void missionUploaded();
};

#endif // SERIALCOMMUNICATION_USB_H
