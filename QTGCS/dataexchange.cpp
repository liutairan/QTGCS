#include "dataexchange.h"

DataExchange::DataExchange(QObject *parent) :
    QObject(parent)
{
    teleSerialOn = false;
    teleMode = 0;
    rcMode = 0;
    manualMode = 0;
    teleSerialPortName = "";
    teleConnectionMethod = "";
    rcSerialPortName = "";
    rcConnectionMethod = "";
    for (int i =0; i<3; i++)
    {
        current_gps[i].lat = 0;
        current_gps[i].lon = 0;

        teleAddressList[i] = "";
        rcAddressList[i] = "";
    }
    initServerWorker();
    initRemoteControl();
    initTelemetryChannel();
}

DataExchange::~DataExchange()
{
    teleSerialOn = false;

    // Send log info to main GUI
    LogMessage tempLogMessage;
    tempLogMessage.id = QString("DataExchange");
    tempLogMessage.message = QString("Deleting thread and worker in Thread.");
    emit logMessageRequest(tempLogMessage);
    //
    //worker->abort();
    teleSerialThread->terminate();
    teleSerialThread->wait();

    qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();

    delete teleSerialWorker;
    delete teleSerialThread;
}

void DataExchange::initServerWorker()
{
    // Initialize local server.
    //    This local server is running in a thread, and is used to
    //    process data come from a connected wireless device, then
    //    broadcast to a drone with connected XBee.
    //    This part is important in case of emergency situations
    //    that operator can operate the drone directly by hand.
    //    This is also required by FAA.
    //serverThread = new QThread();
    //serverWorker = new LocalServerWorker();
    //serverWorker->moveToThread(serverThread);

    //connect(serverWorker, SIGNAL(workRequested()), serverThread, SLOT(start()));
    //connect(serverThread, SIGNAL(started()), serverWorker, SLOT(doWork()));
    // Even though the finished() signal is connected to the server thread,
    //    but this should not be emitted until the end of the whole app.
    //    Otherwise the server will not be able to restart any more.
    //connect(serverWorker, SIGNAL(finished()), serverThread, SLOT(quit()), Qt::DirectConnection);
    //connect(serverWorker, &LocalServerWorker::logMessageRequest, this, &DataExchange::logMessage);
    //serverWorker->requestWork();


    manual_rc_values.rcData[0] = 1500;
    manual_rc_values.rcData[1] = 1500;
    manual_rc_values.rcData[2] = 1000;
    manual_rc_values.rcData[3] = 1500;
    manual_rc_values.rcData[4] = 1000;
    manual_rc_values.rcData[5] = 1000;
    manual_rc_values.rcData[6] = 1000;
    manual_rc_values.rcData[7] = 1000;
    // Even though 8 channels are set here, there is still a possibility
    //    more channels are used. This is depending on the settings.
    //    Traditionally, at most 8 channels were available on the remote
    //    controller. But recently more channels are available.
    //    For example, iNav support 18 channels.

    server = new LocalServer;
    // Server received data from network.
    connect(server, SIGNAL(inputReceived(QString)), this, SLOT(updateRCValues(QString)) );

    // Log message channel
    connect(server, &LocalServer::logMessageRequest, this, &DataExchange::logMessage);
}

void DataExchange::initRemoteControl()
{
    // Initialize aux serial port name and connection method
    //    In future, the connection method should be able to
    //    set with two different ways, point to point and
    //    point to network.
    //    Point to point is manual control mode, and point
    //    to network is autonomous control mode.
    rcSerialPortName = "tty.usbserial-00000000";
    rcConnectionMethod = "AT";

    rcSerialOn = false;
    manualMode = 0;
    rcSerialReady = false;

    // serial remote control output
    rcTimer = new QTimer();
    QObject::connect(rcTimer, SIGNAL(timeout()), this, SLOT(rcSwitch()));
    // Time specified in ms, as the frequency to send out remote control
    //    command to one agent for manual control.
    rcTimer->start(100);
}

void DataExchange::initTelemetryChannel()
{
    teleSerialThread = new QThread();
    teleSerialWorker = new TelemetrySerialWorker();

    // Connect signals and slots
    // QuadStates update channel
    connect(teleSerialWorker, SIGNAL(quadsStatesChangeRequest(QList<QuadStates *> *)), this, SLOT(updateQuadsStates(QList<QuadStates *> *)));
    // Log message channel
    connect(teleSerialWorker, &TelemetrySerialWorker::logMessageRequest, this, &DataExchange::logMessage);
    // Tele mode update channel
    connect(this, SIGNAL(teleSerialOnChanged(bool)), teleSerialWorker, SLOT(teleSerialOnUpdate(bool)));
    connect(this, SIGNAL(teleModeChanged(int)), teleSerialWorker, SLOT(teleModeUpdate(int)));
    connect(teleSerialWorker, SIGNAL(teleModeChangeRequest(int)), this, SLOT(teleModeUpdateFB(int)));

    // Mode worker to thread
    teleSerialWorker->moveToThread(teleSerialThread);
    teleSerialThread->start();
}

void DataExchange::rcSwitch()
{
    if (rcSerialReady)
    {
        rcWorker();
    }
}

void DataExchange::rcWorker()
{
    if (rcSerialReady == true)
    {
        switch (manualMode) {
        case 0:
        {
            break;
        }
        case 1:
        {
            /*qDebug() << manual_rc_values.rcData[0]
                     << manual_rc_values.rcData[1]
                     << manual_rc_values.rcData[2]
                     << manual_rc_values.rcData[3]
                     << manual_rc_values.rcData[4]
                     << manual_rc_values.rcData[5]
                     << manual_rc_values.rcData[6]
                     << manual_rc_values.rcData[7];*/
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Remote Control");
            tempLogMessage.message = "<br/> R " + QString::number(manual_rc_values.rcData[0], 10)
                                   + " P " + QString::number(manual_rc_values.rcData[1], 10)
                                   + " T " + QString::number(manual_rc_values.rcData[2], 10)
                                   + " Y " + QString::number(manual_rc_values.rcData[3], 10)
                                   + "<br/> 5 " + QString::number(manual_rc_values.rcData[4], 10)
                                   + " 6 " + QString::number(manual_rc_values.rcData[5], 10)
                                   + " 7 " + QString::number(manual_rc_values.rcData[6], 10)
                                   + " 8 " + QString::number(manual_rc_values.rcData[7], 10);
            emit logMessageRequest(tempLogMessage);
            //
            //rc_xbee_at = new RemoteControl_XBEE_AT(serial);
            rc_xbee_at->sendCMD(MSP_SET_RAW_RC, manual_rc_values);
            break;
        }
        case 2:
        {
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Remote Control");
            tempLogMessage.message = QString("RC is not set on CH2.");
            emit logMessageRequest(tempLogMessage);
            //
            //qDebug() << "Not set 2";
            break;
        }
        case 3:
        {
            // Send log info to main GUI
            LogMessage tempLogMessage;
            tempLogMessage.id = QString("Remote Control");
            tempLogMessage.message = QString("RC is not set on CH3.");
            emit logMessageRequest(tempLogMessage);
            //
            //qDebug() << "Not set 3";
            break;
        }
        default:
            break;
        }
    }
}

QString DataExchange::get_teleSerialPortName()
{
    return teleSerialPortName;
}

void DataExchange::set_teleSerialPortName(QString value)
{
    teleSerialPortName = value;
    teleSerialWorker->setTelemetrySerialPortName(value);
}

QString DataExchange::get_teleConnectionMethod()
{
    return teleSerialPortName;
}

void DataExchange::set_teleConnectionMethod(QString value)
{
    teleConnectionMethod = value;
    teleSerialWorker->setTelemetryConnectionMethod(value);
}

bool DataExchange::get_teleSerialOn() const
{
    return teleSerialOn;
}

void DataExchange::set_teleSerialOn(bool value)
{
    // The thread for telemetry serial communication is
    //    opened at the begining of the program running.
    //    But the timer is started when the telemetry
    //    connection button is pressed.
    if (value != teleSerialOn)
    {
        teleSerialOn = value;
        if (teleSerialOn == true)
        {
            set_teleSerialPortName(teleSerialPortName);
            set_teleConnectionMethod(teleConnectionMethod);

            //teleSerialWorker->setTelemetrySerialOn(teleSerialOn);
//            teleTimer = new QTimer();
//            QObject::connect(teleTimer, SIGNAL(timeout()), this, SLOT()); // SLOT to be filled.
//            teleTimer->start(1000);
//            qDebug() << teleMode;
            //teleSerialThread = new QThread();
            //worker = new SerialWorker (serialPortName, connectionMethod, addressList);
            //worker->setPortName(serialPortName);

//            worker->moveToThread(thread);
//            connect(worker, SIGNAL(workRequested()), thread, SLOT(start()));
//            connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
//            connect(worker, SIGNAL(finished()), thread, SLOT(quit()), Qt::DirectConnection);

//            connect(worker,SIGNAL(quadsStatesChanged(QList<QuadStates *> *)), this, SLOT(updateQuadsStates(QList<QuadStates *> *)) );
//            connect(worker, &SerialWorker::logMessageRequest, this, &DataExchange::logMessage);
//            worker->requestWork();
        }
        else if (teleSerialOn == false)
        {
            //teleSerialWorker->setTelemetrySerialOn(teleSerialOn);
//            teleTimer->stop();
//            delete teleTimer;
//            worker->abort();
//            thread->wait();
            // Clean up when closed
            //delete worker;
            //delete thread;
        }
        // Report the change to MainWindow
        emit teleSerialOnChanged(teleSerialOn);
    }
    //LogMessage tempLogMessage;
    //tempLogMessage.id = QString("DataExchange");
    //tempLogMessage.message = QString("Test data trans");
    //emit logMessageRequest(tempLogMessage);
    //emit logMessageRequest(QString("Test data trans"));
}


// Telemetry mode changed because of download or upload
//    missions. Modes were changed in TelemetrySerialWorker
//    class, feedback to the DataExchange class.
void DataExchange::teleModeUpdateFB(int mode)
{
    set_teleMode(mode);
}

int DataExchange::get_teleMode() const
{
    return teleMode;
}


void DataExchange::set_teleMode(int value)
{
    //qDebug() << value << teleMode;
    // This set_serialMode is to set the mode of
    //    transmitting normal check data or mission
    //    data or get mission data.
    if (value != teleMode)
    {
        teleMode = value;

        if (teleMode == 11)
        {
            teleSerialWorker->mi_list_air[0] = mi_list_air[0];
        }
        else if (teleMode == 12)
        {
            teleSerialWorker->mi_list_air[1] = mi_list_air[1];
        }
        else if (teleMode == 13)
        {
            teleSerialWorker->mi_list_air[2] = mi_list_air[2];
        }
        // Use this signal instead of using teleSerialWorker.teleModeUpdate,
        //    using teleSerialWorker.teleModeUpdate will cause the app crash
        //    since it is called from another thread, then the timer is not
        //    stopped correctly.
        emit teleModeChanged(teleMode);
    }
}

int DataExchange::get_rcMode() const
{
    return rcMode;
}

void DataExchange::set_rcMode(int value)
{
    if (value != rcMode)
    {
        rcMode = value;
        //worker->radioStatus = rcMode;
    }
}

bool DataExchange::get_rcSerialOn() const
{
    return rcSerialOn;
}

void DataExchange::set_rcSerialOn(bool value)
{
    if (value != rcSerialOn)
    {
        rcSerialOn = value;
        if (rcSerialOn == true)
        {
            set_rcSerialPortName(rcSerialPortName);

            for (int i = 0; i< 3; i++)
            {
                rcAddressList[i] = teleAddressList[i];
            }
            // remote control serial port is opened.
            //    create a new QSerialPort
            rcSerial = new QSerialPort();
            //qDebug() << _serialPortName;
            // To do: the serial port name may changed, need to check
            //    the port name again.
            rcSerial->setPortName(rcSerialPortName);
            rc_xbee_at = new RemoteControl_XBEE_AT(rcSerial);
        }
        else if (rcSerialOn == false)
        {
            set_rcSerialPortName("");
            if (rcSerial->isOpen())
            {
                rcSerial->close();
            }
            delete rcSerial;
        }
        emit rcSerialOnChanged(rcSerialOn);
    }
}

int DataExchange::get_manualMode() const
{
    return manualMode;
}

void DataExchange::set_manualMode(int value)
{
    if (value != manualMode)
    {
        manualMode = value;
        //qDebug() << _manualMode;
        if (manualMode > 0)
        {
            rcSerialReady = true;
        }
        else
        {
            rcSerialReady = false;
        }
        //
        LogMessage tempLogMessage;
        tempLogMessage.id = QString("DataExchange");
        tempLogMessage.message = QString("Manual Mode:") + QString::number(manualMode, 10);
        emit logMessageRequest(tempLogMessage);
    }
}

QString DataExchange::get_rcSerialPortName()
{
    //return _serialPortName;
    return rcSerialPortName;
}

void DataExchange::set_rcSerialPortName(QString value)
{
    //_serialPortName = value;
    rcSerialPortName = value;
    //qDebug() << "set serial port name" << _serialPortName;
}

void DataExchange::updateRCValues(QString msg)
{
    //qDebug() << msg;
    if (msg.length() > 0)
    {
        QStringList msgFields = msg.split(":");

        if (msgFields.at(0) == "thr")
        {
            try
            {
                int tempValue = msgFields.at(1).toInt();
                if ( (tempValue >= 1000) && (tempValue <= 2000) )
                {
                    manual_rc_values.rcData[2] = tempValue;
                }
            }
            catch (...)
            {
                qDebug() << "Throttle value error";
                //
                //tempMessage.id = "LocalServerWorker";
                //tempMessage.message = "Throttle value error";
                //emit logMessageRequest(tempMessage);
                //
                foreach (QString tempStr, msgFields) {
                    qDebug() << tempStr;
                }
            }

        }
        else if (msgFields.at(0) == "yaw")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -180.0) && (tempValue <= 180.0) )
            {
                manual_rc_values.rcData[3] = mapAngleToPWM(tempValue, -180.0, 180.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "pitch")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -90.0) && (tempValue <= 90.0) )
            {
                manual_rc_values.rcData[1] = mapAngleToPWM(tempValue, -90.0, 90.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "roll")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -90.0) && (tempValue <= 90.0) )
            {
                manual_rc_values.rcData[0] = mapAngleToPWM(tempValue, -90.0, 90.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "aux1")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[4] = 1350;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[4] = 1000;
            }
        }
        else if (msgFields.at(0) == "aux2")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[5] = 1000;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[5] = 1100;
            }
        }
        else if (msgFields.at(0) == "aux3")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[6] = 1800;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[6] = 1000;
            }
        }
        else if (msgFields.at(0) == "aux4")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[7] = 1800;
            }
            else if (tempValue == 2)
            {
                manual_rc_values.rcData[7] = 1600;
            }
            else if (tempValue == 3)
            {
                manual_rc_values.rcData[7] = 1400;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[7] = 1000;
            }
        }
    }
}

uint16_t DataExchange::mapAngleToPWM(float realAngle, float minAngle, float maxAngle, uint16_t minPWM, uint16_t maxPWM)
{
    uint16_t realPWM = int((maxPWM - minPWM)*(realAngle - minAngle)/(maxAngle - minAngle) + minPWM);
    return realPWM;
}

void DataExchange::updateQuadsStates(QList<QuadStates *> *tempObjList)
{
    for (int i = 0; i < tempObjList->length(); i++)
    {
        current_gps[i].lat = tempObjList->at(i)->msp_raw_gps.gpsSol_llh_lat/qPow(10.0, 7);
        current_gps[i].lon = tempObjList->at(i)->msp_raw_gps.gpsSol_llh_lon/qPow(10.0, 7);
    }
    emit quadsStatesChangeRequest(tempObjList);
}

void DataExchange::logMessage(LogMessage tempMessage)
{
    emit logMessageRequest(tempMessage);
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
    manualMode = 0;
    for (int i =0; i<3; i++)
    {
        addressList[i] = addrList[i];
    }
    qRegisterMetaType<LogMessage>("LogMessage");
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
    //
    LogMessage tempLogMessage;
    tempLogMessage.id = "SerialWorker";
    tempLogMessage.message = "Request worker start in Thread ";
    emit logMessageRequest(tempLogMessage);
    //
    mutex.unlock();

    emit workRequested();
}

void SerialWorker::abort()
{
    mutex.lock();
    if (_working) {
        _abort = true;
        qDebug()<<"Request worker aborting in Thread "<<thread()->currentThreadId();
        //
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialWorker";
        tempLogMessage.message = "Request worker aborting in Thread ";
        emit logMessageRequest(tempLogMessage);
        //
    }
    mutex.unlock();
}

void SerialWorker::doWork()
{
    qDebug()<<"Starting worker process in Thread "<<thread()->currentThreadId();
    //
    LogMessage tempLogMessage;
    tempLogMessage.id = "SerialWorker";
    tempLogMessage.message = "Starting worker process in Thread ";
    emit logMessageRequest(tempLogMessage);
    //
    mutex.lock();
    bool abort = _abort;
    mutex.unlock();

    serial = new QSerialPort;
    serial->setPortName(serialPortName);

    // connection to serial communication handle
    scHandle = new SerialCommunication(serial, connectionMethod, addressList);
    connect(scHandle,SIGNAL(quadsStatesChangeRequest(QList<QuadStates *> *)), this, SLOT(updateQuadsStates(QList<QuadStates *> *)) );
    // connect for log message
    connect(scHandle, &SerialCommunication::logMessageRequest, this, &SerialWorker::logMessage);
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
            //
            //LogMessage tempLogMessage;
            tempLogMessage.id = "SerialWorker";
            tempLogMessage.message = "Aborting worker process in Thread ";
            emit logMessageRequest(tempLogMessage);
            //
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
    //
    //LogMessage tempLogMessage;
    tempLogMessage.id = "SerialWorker";
    tempLogMessage.message = "Worker process finished in Thread ";
    emit logMessageRequest(tempLogMessage);
    //
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
            // need to check the logic of this function,
            //    whether allow any one of three ports to
            //    connect or only allow the first port to connect
            switch (modeSelection) {
            case 0:  // check the quad connected and all fields of information, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                break;
            }
            case 1:  // if connected, check quad 1 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                break;
            }
            case 2:  // if connected, check quad 2 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                break;
            }
            case 3:  // if connected, check quad 3 and all gps, send out radio control signal
            {
                // check
                scHandle->RegularCheck();
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                break;
            }
            case 11:  // if connected, upload missions, then return to normal check mode
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
                QTime dieTime1 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2 = QTime::currentTime().addMSecs(50);
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
                QTime dieTime1 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                // radio
                scHandle->RegularArmAndNavAll(radioStatus);
                QTime dieTime2 = QTime::currentTime().addMSecs(50);
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


void SerialWorker::logMessage(LogMessage tempMessage)
{
    emit logMessageRequest(tempMessage);
}

LocalServerWorker::LocalServerWorker(QObject *parent) :
    QObject(parent)
{
    _serialPortName = "tty.usbserial-AL00R598";
    connectionMethod = "AT";

    _working = false;
    _abort = false;
    _auxSerialOn = false;
    _manualMode = 0;
    serialReady = false;
    manual_rc_values.rcData[0] = 1500;
    manual_rc_values.rcData[1] = 1500;
    manual_rc_values.rcData[2] = 1000;
    manual_rc_values.rcData[3] = 1500;
    manual_rc_values.rcData[4] = 1000;
    manual_rc_values.rcData[5] = 1000;
    manual_rc_values.rcData[6] = 1000;
    manual_rc_values.rcData[7] = 1000;

    server = new LocalServer;
    connect(server, SIGNAL(inputReceived(QString)), this, SLOT(updateRCValues(QString)) );
    qRegisterMetaType<LogMessage>("LogMessage");
    qDebug()<<"LS in Thread "<<thread()->currentThreadId();
}

void LocalServerWorker::requestWork()
{
    mutex.lock();
    //_working = true;
    _abort = false;
    qDebug()<<"Request server worker start in Thread "<<thread()->currentThreadId();
    // Log
    LogMessage tempLogMessage;
    tempLogMessage.id = "LocalServerWorker";
    tempLogMessage.message = "Request server worker start in Thread ";
    emit logMessageRequest(tempLogMessage);
    //
    mutex.unlock();

    emit workRequested();
}

void LocalServerWorker::abort()
{
    // This function is not used.
    //    This thread is a little different with the thread of main serial com.
    mutex.lock();
    if (_working) {
        _abort = true;
        _working = false;
        qDebug()<<"Request server worker aborting in Thread "<<thread()->currentThreadId();
        //
        LogMessage tempLogMessage;
        tempLogMessage.id = "LocalServerWorker";
        tempLogMessage.message = "Request server worker aborting in Thread ";
        emit logMessageRequest(tempLogMessage);
        //
    }
    mutex.unlock();
}

void LocalServerWorker::serverSwitch()
{
    if (serialReady)
    {
        realWorker();
    }
}

void LocalServerWorker::doWork()
{
    qDebug()<<"Starting server worker process in Thread "<< QThread::currentThreadId();
    // Log message
    LogMessage tempLogMessage;
    tempLogMessage.id = "LocalServerWorker";
    tempLogMessage.message = "Starting server worker process in Thread ";
    emit logMessageRequest(tempLogMessage);
    //
    // serial remote control output
    timer = new QTimer();
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(serverSwitch()));
    // Time specified in ms, as the frequency to send out remote control
    //    command to one agent for manual control.
    timer->start(500);
    //serverSwitch();
}

void LocalServerWorker::updateRCValues(QString msg)
{
    if (msg.length() > 0)
    {
        QStringList msgFields = msg.split(":");

        if (msgFields.at(0) == "thr")
        {
            try
            {
                int tempValue = msgFields.at(1).toInt();
                if ( (tempValue >= 1000) && (tempValue <= 2000) )
                {
                    manual_rc_values.rcData[2] = tempValue;
                }
            }
            catch (...)
            {
                qDebug() << "Throttle value error";
                //
                LogMessage tempLogMessage;
                tempLogMessage.id = "LocalServerWorker";
                tempLogMessage.message = "Throttle value error";
                emit logMessageRequest(tempLogMessage);
                //
                foreach (QString tempStr, msgFields) {
                    qDebug() << tempStr;
                }
            }

        }
        else if (msgFields.at(0) == "yaw")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -180.0) && (tempValue <= 180.0) )
            {
                manual_rc_values.rcData[3] = mapAngleToPWM(tempValue, -180.0, 180.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "pitch")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -90.0) && (tempValue <= 90.0) )
            {
                manual_rc_values.rcData[1] = mapAngleToPWM(tempValue, -90.0, 90.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "roll")
        {
            int tempValue = msgFields.at(1).toFloat();
            if ( (tempValue >= -90.0) && (tempValue <= 90.0) )
            {
                manual_rc_values.rcData[0] = mapAngleToPWM(tempValue, -90.0, 90.0, 1000, 2000);
            }
        }
        else if (msgFields.at(0) == "aux1")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[4] = 1350;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[4] = 1000;
            }
        }
        else if (msgFields.at(0) == "aux2")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[5] = 1000;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[5] = 1100;
            }
        }
        else if (msgFields.at(0) == "aux3")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[6] = 1800;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[6] = 1000;
            }
        }
        else if (msgFields.at(0) == "aux4")
        {
            int tempValue = msgFields.at(1).toInt();
            if (tempValue == 1)
            {
                manual_rc_values.rcData[7] = 1800;
            }
            else if (tempValue == 2)
            {
                manual_rc_values.rcData[7] = 1600;
            }
            else if (tempValue == 3)
            {
                manual_rc_values.rcData[7] = 1400;
            }
            else if (tempValue == 0)
            {
                manual_rc_values.rcData[7] = 1000;
            }
        }
    }
}

uint16_t LocalServerWorker::mapAngleToPWM(float realAngle, float minAngle, float maxAngle, uint16_t minPWM, uint16_t maxPWM)
{
    uint16_t realPWM = int((maxPWM - minPWM)*(realAngle - minAngle)/(maxAngle - minAngle) + minPWM);
    return realPWM;
}

void LocalServerWorker::realWorker()
{
    if (serialReady == true)
    {
        switch (_manualMode) {
        case 0:
        {
            break;
        }
        case 1:
        {
            qDebug() << manual_rc_values.rcData[0]
                     << manual_rc_values.rcData[1]
                     << manual_rc_values.rcData[2]
                     << manual_rc_values.rcData[3]
                     << manual_rc_values.rcData[4]
                     << manual_rc_values.rcData[5]
                     << manual_rc_values.rcData[6]
                     << manual_rc_values.rcData[7];
            qDebug()<<"in Thread "<<thread()->currentThreadId();
            qDebug() << "Locate 1";
            //rc_xbee_at = new RemoteControl_XBEE_AT(serial);
            rc_xbee_at->sendCMD(MSP_SET_RAW_RC, manual_rc_values);
            qDebug() << "Locate 2";
            break;
        }
        case 2:
        {
            qDebug() << "Not set 2";
            break;
        }
        case 3:
        {
            qDebug() << "Not set 3";
            break;
        }
        default:
            break;
        }
        // Try to use signal rather than while loop.
        /*QTime dieTime1= QTime::currentTime().addMSecs(50);

        while( QTime::currentTime() < dieTime1 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }*/
    }
}

QString LocalServerWorker::get_serialPortName() const
{
    return _serialPortName;
}

void LocalServerWorker::set_serialPortName(QString value)
{
    _serialPortName = value;
    //qDebug() << "set serial port name" << _serialPortName;
}

bool LocalServerWorker::get_auxSerialOn() const
{
    return _auxSerialOn;
}

void LocalServerWorker::set_auxSerialOn(bool value)
{
    _auxSerialOn = value;
    if (_auxSerialOn == true)
    {
        // aux serial port is opened.
        //    create a new QSerialPort
        serial = new QSerialPort();
        //qDebug() << _serialPortName;
        // To do: the serial port name may changed, need to check
        //    the port name again.
        serial->setPortName(_serialPortName);
        rc_xbee_at = new RemoteControl_XBEE_AT(serial);
        _working = true;
    }
    else if (_auxSerialOn == false)
    {
        // To do: implement another part to change the manual mode to 0
        //    when the aux serial port is closed.
        if (serial->isOpen())
        {
            serial->close();
        }
        delete serial;

        // Do not emit finished() signal, otherwise the thread will not able to restart again.
        //emit finished();
        _working = false;
    }
}

int LocalServerWorker::get_manualMode() const
{
    return _manualMode;
}

void LocalServerWorker::set_manualMode(int value)
{
    _manualMode = value;
    if (_auxSerialOn == false)
    {
        set_auxSerialOn(true);
    }
    qDebug() << "Watch manual mode" << _auxSerialOn << _manualMode;
    if ((_auxSerialOn == true) && (_manualMode > 0))
    {
        QuadStates *tempQS;
        tempQS = new QuadStates(QByteArray::fromHex("1"),
                                QByteArray::fromHex("0013a20040c14306"),
                                QByteArray::fromHex("fffe"));
        quadstates_list.append(tempQS);
        //sc_xbee_at = new SerialCommunication_XBEE_AT(serial, quadstates_list);
        qDebug()<<"in Thread "<<thread()->currentThreadId();

        serialReady = true;
    }
    else
    {
        serialReady = false;
        if (_manualMode == 0)
        {
            /*if (serial->isOpen())
            {
                serial->close();
            }
            delete rc_xbee_at;*/
        }
    }
}
