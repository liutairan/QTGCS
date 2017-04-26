#include "serialcommunication.h"

SerialCommunication::SerialCommunication(QObject *parent) :
    QObject(parent)
{}

SerialCommunication::SerialCommunication(QSerialPort *ser, QString connMethod, QString addrList[])
{
    serial = ser;
    connectionMethod = connMethod;
    for (int i =0; i<3; i++)
    {
        addressList[i] = addrList[i];
    }

    if (connectionMethod == "USB")
    {
        QuadStates *tempQS;
        tempQS = new QuadStates(QByteArray::fromHex("1"),
                                QByteArray::fromHex("0013a20040c14306"),
                                QByteArray::fromHex("fffe"));
        quadstates_list.append(tempQS);
        sc_usb = new SerialCommunication_USB(serial, quadstates_list);
        connect(sc_usb,SIGNAL(qsReady(QList<QuadStates *> *)), this, SLOT(Update(QList<QuadStates *> *)) );
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        for (int i=0; i<3; i++)
        {
            if (addressList[i].length() > 2)
            {
                QuadStates *tempQS;
                tempQS = new QuadStates(QByteArray::fromHex(QString::number(i+1,10).toUtf8()),
                                        QByteArray::fromHex(addressList[i].toUtf8()),
                                        QByteArray::fromHex("fffe"));
                quadstates_list.append(tempQS);
            }
        }
        sc_xbee_api = new SerialCommunication_XBEE_API(serial, quadstates_list);
        connect(sc_xbee_api,SIGNAL(qsReady(QList<QuadStates *> *)), this, SLOT(Update(QList<QuadStates *> *)) );
    }
}

SerialCommunication::~SerialCommunication()
{}

void SerialCommunication::send(TXRequest *request){}
void SerialCommunication::broadcast(QString data){}
void SerialCommunication::unicast(QByteArray address, QString data){}

void SerialCommunication::stopSerial()
{}

void SerialCommunication::createObjs()
{}

void SerialCommunication::PreLoadInfo()
{
    PreCheck();
}

void SerialCommunication::PreCheck()
{
    if (connectionMethod == "USB")
    {
        sc_usb->sendCMD(MSP_BOXIDS);
        QTime dieTime= QTime::currentTime().addMSecs(500);
        while( QTime::currentTime() < dieTime )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        for (int i=0; i<3; i++)
        {
            if (addressList[i].length() > 2)
            {
                sc_xbee_api->sendCMD(i, MSP_BOXIDS);
                QTime dieTime = QTime::currentTime().addMSecs(1000);
                while( QTime::currentTime() < dieTime )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
            }
        }
    }
}

void SerialCommunication::RegularLoadInfo()
{
    RegularCheck();
}

void SerialCommunication::RegularCheck()
{
    if (connectionMethod == "USB")
    {
        sc_usb->sendCMD(MSP_STATUS_EX);
        QTime dieTime1 = QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime1 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
        sc_usb->sendCMD(MSP_ATTITUDE);
        QTime dieTime2 = QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime2 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
        sc_usb->sendCMD(MSP_RAW_GPS);
        QTime dieTime3 = QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime3 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        for (int i=0; i<3; i++)
        {
            if (addressList[i].length() > 2)
            {
                sc_xbee_api->sendCMD(i, MSP_STATUS_EX);
                QTime dieTime1 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                /*
                sc_xbee_api->sendCMD(i, MSP_ATTITUDE);
                QTime dieTime2 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
                */
                sc_xbee_api->sendCMD(i, MSP_RAW_GPS);
                QTime dieTime3 = QTime::currentTime().addMSecs(100);
                while( QTime::currentTime() < dieTime3 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }
            }
        }
    }
}

void SerialCommunication::RegularLoadOverview()
{}

void SerialCommunication::RegularLoadAllGPS()
{}

void SerialCommunication::RegularLoadQuad1()
{}

void SerialCommunication::RegularLoadQuad2()
{}

void SerialCommunication::RegularLoadQuad3()
{}

void SerialCommunication::RegularLoadInfoLoose()
{}

void SerialCommunication::UploadMissions(int objInd, Mission_list tempMissionList)
{
    if (connectionMethod == "USB")
    {
        QuadStates *tempQS;
        tempQS = quadstates_list.at(0);
        tempQS->mission_list = tempMissionList;
        quadstates_list.replace(0, tempQS);
        sc_usb->uploadMissions();
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        QuadStates *tempQS;
        tempQS = quadstates_list.at(objInd);
        tempQS->mission_list = tempMissionList;
        quadstates_list.replace(objInd, tempQS);
        sc_xbee_api->uploadMissions(objInd);
    }
}

void SerialCommunication::DownloadMissions(int objInd)
{
    if (connectionMethod == "USB")
    {
        qDebug() << "Download missions via USB";
        sc_usb->downloadMissions();
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        sc_xbee_api->downloadMissions(objInd);
    }
}

void SerialCommunication::RegularArmAndNavAll(int radioMode)
{
    if (connectionMethod == "USB")
    {
        //qDebug() << "via USB";
    }
    else if (connectionMethod == "AT")
    {}
    else if (connectionMethod == "API")
    {
        sc_xbee_api->radioControl(radioMode);
    }
}

void SerialCommunication::RegularArmAll()
{}

void SerialCommunication::RegularDisarmAll()
{}

void SerialCommunication::RegularNavAll()
{}

void SerialCommunication::RegularDisnavAll()
{}

void SerialCommunication::AbortMission()
{}

void SerialCommunication::Update(QList<QuadStates *> *tempObjList)
{
    emit quadsStatesChanged(tempObjList);
}
