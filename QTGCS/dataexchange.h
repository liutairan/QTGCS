#ifndef DATAEXCHANGE_H
#define DATAEXCHANGE_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QEventLoop>
#include <QTimer>
#include <QMutex>
#include <QTime>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "map.h"
#include "localserver.h"
#include "telemetryserialworker.h"
#include "quadstates.h"
#include "serialcommunication.h"
#include "qtxb.h"
#include "serialcommunication_xbee_at.h"
#include "remotecontrol_xbee_at.h"
#include "logmessage.h"
#include "remotecontrol.h"

class DataExchange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool teleSerialOn READ get_teleSerialOn WRITE set_teleSerialOn NOTIFY teleSerialOnChanged)
    Q_PROPERTY(int teleMode READ get_teleMode WRITE set_teleMode NOTIFY teleModeChanged)
    //Q_PROPERTY(QString teleSerialPortName READ teleSerialPortName WRITE setTeleSerialPortName NOTIFY teleSerialPortNameChanged)
    Q_PROPERTY(int rcMode READ get_rcMode WRITE set_rcMode NOTIFY rcModeChanged)
    Q_PROPERTY(bool rcSerialOn READ get_rcSerialOn WRITE set_rcSerialOn NOTIFY rcSerialOnChanged)
    Q_PROPERTY(int manualMode READ get_manualMode WRITE set_manualMode NOTIFY manualModeChanged)
public:
    DataExchange(QObject *parent = nullptr);
    ~DataExchange();

    QString teleSerialPortName;
    QString rcSerialPortName;
    QString teleConnectionMethod;
    QString rcConnectionMethod;

    bool get_teleSerialOn() const;
    void set_teleSerialOn(bool value);
    int get_teleMode() const;
    void set_teleMode(int value);
    int get_rcMode() const;
    void set_rcMode(int value);

    bool get_rcSerialOn() const;
    void set_rcSerialOn(bool value);
    int get_manualMode() const;
    void set_manualMode(int value);

//    QString get_teleSerialPortName() const;
    QString get_teleSerialPortName();
    void set_teleSerialPortName(QString value);
    QString get_teleConnectionMethod();
    void set_teleConnectionMethod(QString value);
//    QString get_rcSerialPortName() const;
    QString get_rcSerialPortName();
    void set_rcSerialPortName(QString value);

    QString teleAddressList[3];
    QString rcAddressList[3];

    WP_list wp_list[3];
    Mission_list mi_list_air[3];
    GPSCoordinate current_gps[3];
    QList<QuadStates *> quadstates_list;

    // Local Server
    LocalServer *server;

    // Serial Port
    QSerialPort *teleSerial;
    QSerialPort *rcSerial;

    RemoteControl_XBEE_AT *rc_xbee_at;

    bool teleSerialReady;
//    bool rcSerialReady;

signals:
    void teleSerialOnChanged(bool);
    void teleModeChanged(int);
    void rcModeChanged(int);
    void rcSerialOnChanged(bool);
    void manualModeChanged(int);
    void quadsStatesChangeRequest(QList<QuadStates *> *);  // update quads states
    void logMessageRequest(LogMessage);
    void logDataRequest(LogMessage);
public slots:
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
    void logMessage(LogMessage);
    void logData(LogMessage);
    //void updateRCValues(QString msg);
    //void rcSwitch();
    void teleModeUpdateFB(int);

private:
    bool teleSerialOn;  // telemetry serial port switch
    bool rcSerialOn;  // remote control serial port switch
    int teleMode;  // telemetry mode
    int rcMode;  // remote control mode
    int manualMode;  // manual mode

    QThread *teleSerialThread;
    TelemetrySerialWorker *teleSerialWorker;
    RemoteControl *rcHandle;

    QTimer *teleTimer;
    void initServerWorker();
    void initRemoteControl();
    void initTelemetryChannel();
};

#endif // DATAEXCHANGE_H
