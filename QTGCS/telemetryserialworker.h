#ifndef TELEMETRYSERIALWORKER_H
#define TELEMETRYSERIALWORKER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QSerialPort>
#include <QSerialPortInfo>

#include "quadstates.h"
#include "serialcommunication.h"
#include "qtxb.h"
#include "serialcommunication_usb.h"
#include "serialcommunication_xbee_at.h"
#include "serialcommunication_xbee_api.h"
#include "logmessage.h"

class TelemetrySerialWorker : public QObject
{
    Q_OBJECT
public:
    explicit TelemetrySerialWorker(QObject *parent = nullptr);
    //TelemetrySerialWorker(/*QString, QString, QString addr[],*/ QObject *parent = 0);
    int teleMode;
    void setTelemetrySerialPortName(QString);
    void setTelemetryConnectionMethod(QString);
    void setTelemetrySerialOn(bool);
    void setTelemetryMode(int);

    QString teleSerialPortName;
    QString teleConnectionMethod;
    QSerialPort *teleSerial;
    SerialCommunication *scHandle;
    QString teleAddressList[3];
    Mission_list mi_list_air[3];
    //LogMessage tempLogMessage;
    void uploadMissionMode();
    void downloadMissionMode();

signals:
    //void finished();
    void quadsStatesChangeRequest(QList<QuadStates *> *);  // update quads states
    void logMessageRequest(LogMessage);
    void teleModeChangeRequest(int);
public slots:
    void teleSerialOnUpdate(bool);
    void teleModeUpdate(int);
    //void doWork();
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
    void logMessage(LogMessage);
    void overviewPageInfoMode();
    //void uploadMissionMode();
private:
    bool teleSerialOn;
    QTimer *teleTimer;
};

#endif // TELEMETRYSERIALWORKER_H
