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

class SerialWorker : public QObject
{
    Q_OBJECT
    QString serialPortName;
    QString connectionMethod;
public:
    SerialWorker(QString, QString, QString addr[], QObject *parent = 0);
    int modeSelection;  // Control data transmission mode: Normal data transmission, mission upload/download.
    int radioStatus;  // Control flight mode: ARM, NAV, RTH.
    int manualMode;  // Use manual control mode: 0 for not using, 1 for controlling quad 1, 2 for quad 2, 3 for quad 3, need aux wireless modual.
    void setPortName(QString);
    void requestWork();
    void abort();
    void realWorker();
    QSerialPort *serial;
    SerialCommunication *scHandle;
    QString addressList[3];
    Mission_list mi_list_air[3];
    //LogMessage tempLogMessage;
private:
    //QTimer timer;
    bool _abort;
    bool _working;
    QMutex mutex;
signals:
    /**
     * @brief This signal is emitted when the Worker request to Work
     * @sa requestWork()
     */
    void workRequested();
    /**
     * @brief This signal is emitted when counted value is changed (every sec)
     */
    void valueChanged(const QString &value);
    /**
     * @brief This signal is emitted when process is finished (either by counting 60 sec or being aborted)
     */
    void finished();
    void quadsStatesChanged(QList<QuadStates *> *);  // update quads states
    void logMessageRequest(LogMessage);
public slots:
    /**
     * @brief Does something
     *
     * Counts 60 sec in this example.
     * Counting is interrupted if #_aborted is set to true.
     */
    void doWork();
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
    void logMessage(LogMessage);
};

class LocalServerWorker : public QObject
{
    Q_OBJECT

    QString connectionMethod;
    Q_PROPERTY(QString serialPortName READ get_serialPortName WRITE set_serialPortName NOTIFY serialPortNameChanged)
    Q_PROPERTY(int auxSerialOn READ get_auxSerialOn WRITE set_auxSerialOn NOTIFY auxSerialOnChanged)
    Q_PROPERTY(int manualMode READ get_manualMode WRITE set_manualMode NOTIFY manualModeChanged)
public:
    LocalServerWorker(QObject *parent = 0);

    void setPortName(QString);
    void requestWork();
    void abort();
    void realWorker();
    uint16_t mapAngleToPWM(float realAngle, float minAngle, float maxAngle, uint16_t minPWM, uint16_t maxPWM);
    LocalServer *server;
    bool _auxSerialOn;
    int _manualMode;
    QString _serialPortName;
    Msp_rc_channels manual_rc_values;

    QSerialPort *serial;
    //SerialCommunication_XBEE_AT *sc_xbee_at;
    RemoteControl_XBEE_AT *rc_xbee_at;
    QString addressList[3];
    QList<QuadStates *> quadstates_list;

    QString get_serialPortName() const;
    void set_serialPortName(QString value);
    bool get_auxSerialOn() const;
    void set_auxSerialOn(bool value);
    int get_manualMode() const;
    void set_manualMode(int value);

    bool serialReady;
    //LogMessage tempLogMessage;

private:
    bool _abort;
    bool _working;
    QMutex mutex;
    QTimer *timer;
    //void serverSwitch(bool);

signals:
    void workRequested();

    void valueChanged(const QString &value);

    void finished();

    void serialPortNameChanged(QString);
    void auxSerialOnChanged(bool);
    void manualModeChanged(int);
    void logMessageRequest(LogMessage);

public slots:
    void doWork();
    void updateRCValues(QString msg);
    void serverSwitch();
};

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
    DataExchange(QObject *parent = 0);
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
    uint16_t mapAngleToPWM(float realAngle, float minAngle, float maxAngle, uint16_t minPWM, uint16_t maxPWM);
    LocalServer *server;
    Msp_rc_channels manual_rc_values;

    // Serial Port
    QSerialPort *teleSerial;
    QSerialPort *rcSerial;

    //SerialCommunication_XBEE_AT *sc_xbee_at;
    RemoteControl_XBEE_AT *rc_xbee_at;

    bool teleSerialReady;
    bool rcSerialReady;

signals:
    void teleSerialOnChanged(bool);
    void teleModeChanged(int);
    void rcModeChanged(int);
    void rcSerialOnChanged(bool);
    void manualModeChanged(int);
    void quadsStatesChangeRequest(QList<QuadStates *> *);  // update quads states
    void logMessageRequest(LogMessage);
public slots:
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
    void logMessage(LogMessage);
    void updateRCValues(QString msg);
    void rcSwitch();
    void teleModeUpdateFB(int);

private:
    bool teleSerialOn;  // telemetry serial port switch
    bool rcSerialOn;  // remote control serial port switch
    int teleMode;  // telemetry mode
    int rcMode;  // remote control mode
    int manualMode;  // manual mode
    //LogMessage tempLogMessage;

    QThread *teleSerialThread;
    TelemetrySerialWorker *teleSerialWorker;

    QTimer *rcTimer;
    QTimer *teleTimer;
    void initServerWorker();
    void initRemoteControl();
    void initTelemetryChannel();
    void rcWorker();
};

#endif // DATAEXCHANGE_H
