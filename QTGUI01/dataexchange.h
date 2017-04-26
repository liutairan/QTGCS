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
#include "quadstates.h"
#include "serialcommunication.h"
#include "qtxb.h"

class SerialWorker : public QObject
{
    Q_OBJECT
    QString serialPortName;
    QString connectionMethod;
public:
    SerialWorker(QString, QString, QString addr[], QObject *parent = 0);
    int modeSelection;
    int radioStatus;
    void setPortName(QString);
    void requestWork();
    void abort();
    void realWorker();
    QSerialPort *serial;
    SerialCommunication *scHandle;
    QString addressList[3];
    Mission_list mi_list_air[3];
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
public slots:
    /**
     * @brief Does something
     *
     * Counts 60 sec in this example.
     * Counting is interrupted if #_aborted is set to true.
     */
    void doWork();
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
};

class DataExchange : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool serialOn READ get_serialOn WRITE set_serialOn NOTIFY serialOnChanged)
    Q_PROPERTY(bool serialMode READ get_serialMode WRITE set_serialMode NOTIFY serialModeChanged)
    Q_PROPERTY(bool radioMode READ get_radioMode WRITE set_radioMode NOTIFY radioModeChanged)
public:
    DataExchange(QObject *parent = 0);
    ~DataExchange();

    QString serialPortName;
    QString connectionMethod;
    bool get_serialOn() const;
    void set_serialOn(bool value);
    int get_serialMode() const;
    void set_serialMode(int value);
    int get_radioMode() const;
    void set_radioMode(int value);

    QString addressList[3];
    WP_list wp_list[3];
    Mission_list mi_list_air[3];
    GPSCoordinate current_gps[3];
signals:
    void serialOnChanged(bool);
    void serialModeChanged(int);
    void radioModeChanged(int);
    void quadsStatesChanged(QList<QuadStates *> *);  // update quads states
public slots:
    void updateQuadsStates(QList<QuadStates *> *);  // update quads states
private:
    bool _serialOn;
    int _serialMode;
    int _radioMode;
    QThread *thread;
    SerialWorker *worker;
};

#endif // DATAEXCHANGE_H
