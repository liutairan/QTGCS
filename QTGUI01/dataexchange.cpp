#include "dataexchange.h"

DataExchange::DataExchange(QObject *parent) :
    QObject(parent)
{
    _serialOn = false;
    _serialMode = 0;
    _radioMode = 0;
    serialPortName = "";
    connectionMethod = "";
    for (int i =0; i<3; i++)
    {
        current_gps[i].lat = 0;
        current_gps[i].lon = 0;

        addressList[i] = "";
    }
}

DataExchange::~DataExchange()
{
    _serialOn = false;

    worker->abort();
    thread->wait();
    qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
    delete thread;
    delete worker;
}

bool DataExchange::get_serialOn() const
{
    return _serialOn;
}

void DataExchange::set_serialOn(bool value)
{
    if (value != _serialOn)
    {
        _serialOn = value;
        if (_serialOn == true)
        {
            thread = new QThread();
            worker = new SerialWorker (serialPortName, connectionMethod, addressList);
            //worker->setPortName(serialPortName);

            worker->moveToThread(thread);
            //connect(worker, SIGNAL(valueChanged(QString)), ui->label, SLOT(setText(QString)));
            connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
            connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
            connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);

            connect(worker,SIGNAL(quadsStatesChanged(QList<QuadStates *> *)), this, SLOT(updateQuadsStates(QList<QuadStates *> *)) );

            worker->requestWork();
        }
        else if (_serialOn == false)
        {
            worker->abort();
            thread->wait();
        }
        emit serialOnChanged(_serialOn);
    }
}

int DataExchange::get_serialMode() const
{
    return _serialMode;
}

void DataExchange::set_serialMode(int value)
{
    if (value != _serialMode)
    {
        _serialMode = value;
        worker->modeSelection = _serialMode;
        if (_serialMode == 11)
        {
            worker->mi_list_air[0] = mi_list_air[0];
        }
        else if (_serialMode == 12)
        {
            worker->mi_list_air[1] = mi_list_air[1];
        }
        else if (_serialMode == 13)
        {
            worker->mi_list_air[2] = mi_list_air[2];
        }
    }
}

int DataExchange::get_radioMode() const
{
    return _radioMode;
}

void DataExchange::set_radioMode(int value)
{
    if (value != _radioMode)
    {
        _radioMode = value;
        worker->radioStatus = _radioMode;
    }
}

void DataExchange::updateQuadsStates(QList<QuadStates *> *tempObjList)
{
    for (int i = 0; i < tempObjList->length(); i++)
    {
        current_gps[i].lat = tempObjList->at(i)->msp_raw_gps.gpsSol_llh_lat/qPow(10.0, 7);
        current_gps[i].lon = tempObjList->at(i)->msp_raw_gps.gpsSol_llh_lon/qPow(10.0, 7);
    }
    emit quadsStatesChanged(tempObjList);
}

SerialWorker::SerialWorker(QString portName, QString connectionName, QString addrList[], QObject *parent) :
    QObject(parent)
{
    serialPortName = portName;
    connectionMethod = connectionName;
    _working =false;
    _abort = false;
    modeSelection = 0;
    radioStatus = 0;
    for (int i =0; i<3; i++)
    {
        addressList[i] = addrList[i];
    }
}

void SerialWorker::setPortName(QString portName)
{
    serialPortName = portName;
}

void SerialWorker::requestWork()
{
    mutex.lock();
    _working = true;
    _abort = false;
    qDebug()<<"Request worker start in Thread "<<thread()->currentThreadId();
    mutex.unlock();

    emit workRequested();
}

void SerialWorker::abort()
{
    mutex.lock();
    if (_working) {
        _abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
    }
    mutex.unlock();
}

void SerialWorker::doWork()
{
    qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();

    mutex.lock();
    bool abort = _abort;
    mutex.unlock();

    serial = new QSerialPort;
    serial->setPortName(serialPortName);

    // connection to serial communication handle
    scHandle = new SerialCommunication(serial, connectionMethod, addressList);
    connect(scHandle,SIGNAL(quadsStatesChanged(QList<QuadStates *> *)), this, SLOT(updateQuadsStates(QList<QuadStates *> *)) );
    // preload required info
    scHandle->PreLoadInfo();

    while (!abort)
    {
        realWorker();

        mutex.lock();
        bool abort = _abort;
        mutex.unlock();
        if (abort) {
            qDebug()<<"Aborting worker process in Thread "<<thread()->currentThreadId();
            break;
        }
    }
    serial->flush();
    if(serial->isOpen())
    {
        serial->close();
    }

    delete serial;
    // Set _working to false, meaning the process can't be aborted anymore.
    mutex.lock();
    _working = false;
    mutex.unlock();

    qDebug()<<"Worker process finished in Thread "<<thread()->currentThreadId();

    emit finished();
}

void SerialWorker::realWorker()
{
    // regular load info
    if (serial->isOpen() && serial->isWritable())
    {
        //qDebug() << "Serial is open";
        if (connectionMethod == "USB")  // super high update frequency and all data fields, use normal serial communication.
        {
            switch (modeSelection) {
            case 0:
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime1= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 1:  // check quad 1 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime1= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 2:  // not used here
            {
                break;
            }
            case 3:  // not used here
            {
                break;
            }
            case 11:
            {
                scHandle->UploadMissions(0, mi_list_air[0]);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 10;
                break;
            }
            case 12:  // not used here
            {
                break;
            }
            case 13:  // not used here
            {
                break;
            }
            case 21:
            {
                scHandle->DownloadMissions(0);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 20;
                break;
            }
            case 22:  // not used here
            {
                break;
            }
            case 23:  // not used here
            {
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (connectionMethod == "API") // super low update frequency and only very little data fields, need to use API method.
        {
            switch (modeSelection) {
            case 0:
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime1 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 1:  // check quad 1 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 2:  // not used here
            {
                break;
            }
            case 3:  // not used here
            {
                break;
            }
            case 11:
            {
                scHandle->UploadMissions(0, mi_list_air[0]);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 10;
                break;
            }
            case 12:  // not used here
            {
                break;
            }
            case 13:  // not used here
            {
                break;
            }
            case 21:
            {
                scHandle->DownloadMissions(0);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 20;
                break;
            }
            case 22:  // not used here
            {
                break;
            }
            case 23:  // not used here
            {
                break;
            }
            default:
            {
                break;
            }
            }
        }
        else if (connectionMethod == "AT") // medium update frequency and limitted data fields, use normal serial communication.
        {
            switch (modeSelection) {
            case 0:
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime1= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 1:  // check quad 1 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                QTime dieTime1= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2= QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                break;
            }
            case 2:  // not used here
            {
                break;
            }
            case 3:  // not used here
            {
                break;
            }
            case 11:
            {
                scHandle->UploadMissions(0, mi_list_air[0]);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 10;
                break;
            }
            case 12:  // not used here
            {
                break;
            }
            case 13:  // not used here
            {
                break;
            }
            case 21:
            {
                scHandle->DownloadMissions(0);
                QTime dieTime= QTime::currentTime().addMSecs(500);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                modeSelection = modeSelection - 20;
                break;
            }
            case 22:  // not used here
            {
                break;
            }
            case 23:  // not used here
            {
                break;
            }
            default:
            {
                break;
            }
            }
        }
    }
}

void SerialWorker::updateQuadsStates(QList<QuadStates *> *tempObjList)
{
    emit quadsStatesChanged(tempObjList);
}
