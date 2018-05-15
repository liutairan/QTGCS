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

//    Server received data -> RemoteControl
//    -> updateRCValues -> set 3 RC data groups
//    -> reset 1 group with manual input values
//    -> send to XBee.
void DataExchange::initServerWorker()
{
    // Initialize local server.
    server = new LocalServer;

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
    //rcSerialReady = false;
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
            for (uint i=0; i<3; i++)
            {
                teleSerialWorker->teleAddressList[i] = teleAddressList[i];
            }
        }
        else if (teleSerialOn == false)
        {
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
        rcHandle->autoMode = value;
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

            // Create RemoteControl Handle
            //    Serial port is opened in RemoteControl class at the beginning
            rcHandle = new RemoteControl(rcSerialPortName, rcConnectionMethod, rcAddressList);

            // Server received data from network.
            connect(server, SIGNAL(inputReceived(QString)), rcHandle, SLOT(updateRCValues(QString)) );
        }
        else if (rcSerialOn == false)
        {
            // Server received data from network, disconnect
            disconnect(server, SIGNAL(inputReceived(QString)), rcHandle, SLOT(updateRCValues(QString)) );
            set_rcSerialPortName("");
            rcHandle->stop();
            rcHandle->deleteLater();
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
        rcHandle->manualMode = value;

        LogMessage tempLogMessage;
        tempLogMessage.id = QString("DataExchange");
        tempLogMessage.message = QString("Manual Mode:") + QString::number(manualMode, 10);
        emit logMessageRequest(tempLogMessage);
    }
}

QString DataExchange::get_rcSerialPortName()
{
    return rcSerialPortName;
}

void DataExchange::set_rcSerialPortName(QString value)
{
    rcSerialPortName = value;
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
