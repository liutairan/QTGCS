#include "serialcommunication.h"

SerialCommunication::SerialCommunication(QObject *parent) :
    QObject(parent)
{}

SerialCommunication::SerialCommunication(QSerialPort *ser, QString connMethod, QString addrList[])
{
    serial = ser;
    connectionMethod = connMethod;
    everRunFlag = false;
    tReceive.start();
    for (int i =0; i<3; i++)
    {
        addressList[i] = addrList[i];
    }

    if (connectionMethod == "USB")
    {
        //
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication";
        tempLogMessage.message = "USB Initialize";
        emit logMessageRequest(tempLogMessage);
        //
        QuadStates *tempQS;
        tempQS = new QuadStates(QByteArray::fromHex("1"),
                                QByteArray::fromHex("0013a20040c14306"),
                                QByteArray::fromHex("fffe"));
        quadstates_list.append(tempQS);
        sc_usb = new SerialCommunication_USB(serial, quadstates_list);
        connect(sc_usb, SIGNAL(qsReady(QList<QuadStates *> *)), this, SLOT(Update(QList<QuadStates *> *)) );
        connect(sc_usb, SIGNAL(logMessageRequest(LogMessage)), this, SLOT(logMessage(LogMessage)));
    }
    else if (connectionMethod == "AT")
    {
        //
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication";
        tempLogMessage.message = "AT Initialize";
        emit logMessageRequest(tempLogMessage);
        //
        // Get address of quad connected.
        // For AT mode, the number of valid connection
        //    should be exact 1.
        for (uint i = 0; i<3; i++)
        {
            if (addressList[i] != "")
            {
                connectedAddrList.append(addressList[i]);
            }
        }
        QuadStates *tempQS;
        tempQS = new QuadStates(QByteArray::fromHex("1"),
                                QByteArray::fromHex(connectedAddrList.at(0).toUtf8()),
                                QByteArray::fromHex("fffe"));
        quadstates_list.append(tempQS);
        sc_xbee_at = new SerialCommunication_XBEE_AT(serial, quadstates_list);
        connect(sc_xbee_at, SIGNAL(qsReady(QList<QuadStates *> *)), this, SLOT(Update(QList<QuadStates *> *)) );
        connect(sc_xbee_at, SIGNAL(logMessageRequest(LogMessage)), this, SLOT(logMessage(LogMessage)));
    }
    else if (connectionMethod == "API")
    {
        for (int i=0; i<3; i++)
        {
            if (addressList[i] != "")
            {
                connectedAddrList.append(addressList[i]);
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
{
}

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
    everRunFlag = true;
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
    {
        sc_xbee_at->sendCMD(MSP_BOXIDS);
        QTime dieTime= QTime::currentTime().addMSecs(500);
        while( QTime::currentTime() < dieTime )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    else if (connectionMethod == "API")
    {
        for (uint i=0; i<connectedAddrList.length(); i++)
        {
            if (connectedAddrList.at(i) != "")
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
        // Check time elaspe
//        if ((tReceive.elapsed() > 200) && (everRunFlag == true))
//        {
//            QTime dieTimeSleep = QTime::currentTime().addMSecs(1000);
//            while( QTime::currentTime() < dieTimeSleep )
//            {
//                QEventLoop loop;
//                QTimer::singleShot(1, &loop, SLOT(quit()));
//                loop.exec();
//            }
//            qDebug() << "Restart serial communication";
//        }

        // MSP_IDENT
        /*sc_usb->sendCMD(MSP_IDENT);
        QTime dieTime0 = QTime::currentTime().addMSecs(30);
        while( QTime::currentTime() < dieTime0 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }*/

        // MSP_STATUS_EX
        try
        {
            sc_usb->sendCMD(MSP_STATUS_EX);
        }
        catch (...)
        {}

        QTime dieTime1 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime1 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_ANALOG
        try
        {
            sc_usb->sendCMD(MSP_ANALOG);
        }
        catch (...)
        {}

        QTime dieTime2 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime2 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_SONAR_ALTITUDE
        try
        {
            if( quadstates_list.at(0)->msp_sensor_flags.sonar)
            {
                sc_usb->sendCMD(MSP_SONAR_ALTITUDE);
            }
            else // if sonar is not available, still wait for 5ms
            {
            }
        }
        catch (...)
        {}
        QTime dieTime3 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime3 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_ATTITUDE
        try
        {
            sc_usb->sendCMD(MSP_ATTITUDE);
        }
        catch (...)
        {}

        QTime dieTime4 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime4 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_RAW_GPS
        try
        {
            if (quadstates_list.at(0)->msp_sensor_flags.gps)
            {
                sc_usb->sendCMD(MSP_RAW_GPS);
            }
            else // if gps is not available, still wait for 5ms
            {
            }
        }
        catch (...)
        {}
        QTime dieTime5 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime5 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    else if (connectionMethod == "AT")
    {
        // MSP_IDENT
        /*sc_xbee_at->sendCMD(MSP_IDENT);
        QTime dieTime0 = QTime::currentTime().addMSecs(30);
        while( QTime::currentTime() < dieTime0 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }*/

        // MSP_STATUS_EX
        try
        {
            sc_xbee_at->sendCMD(MSP_STATUS_EX);
        }
        catch (...)
        {}

        QTime dieTime1 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime1 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_ANALOG
        try
        {
            sc_xbee_at->sendCMD(MSP_ANALOG);
        }
        catch (...)
        {}

        QTime dieTime2 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime2 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_SONAR_ALTITUDE
        try
        {
            if( quadstates_list.at(0)->msp_sensor_flags.sonar)
            {
                sc_xbee_at->sendCMD(MSP_SONAR_ALTITUDE);
            }
            else // if sonar is not available, still wait for 5ms
            {
            }
        }
        catch (...)
        {}
        QTime dieTime3 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime3 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_ATTITUDE
        try
        {
            sc_xbee_at->sendCMD(MSP_ATTITUDE);
        }
        catch (...)
        {}

        QTime dieTime4 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime4 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }

        // MSP_RAW_GPS
        try
        {
            if (quadstates_list.at(0)->msp_sensor_flags.gps)
            {
                sc_xbee_at->sendCMD(MSP_RAW_GPS);
            }
            else // if gps is not available, still wait for 5ms
            {
            }
        }
        catch (...)
        {}
        QTime dieTime5 = QTime::currentTime().addMSecs(20);
        while( QTime::currentTime() < dieTime5 )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    else if (connectionMethod == "API")
    {
        for (int i=0; i<connectedAddrList.length(); i++)
        {
            if (connectedAddrList.at(i) != "")
            {
                // MSP_IDENT
                /*sc_xbee_api->sendCMD(i, MSP_IDENT);
                QTime dieTime0 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime0 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }*/

                // MSP_STATUS_EX
                try
                {
                    sc_xbee_api->sendCMD(i, MSP_STATUS_EX);
                }
                catch (...)
                {}

                QTime dieTime1 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime1 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }

                // MSP_ANALOG
                try
                {
                    sc_xbee_api->sendCMD(i, MSP_ANALOG);
                }
                catch (...)
                {}

                QTime dieTime2 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime2 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }

                // MSP_SONAR_ALTITUDE
                try
                {
                    if( quadstates_list.at(i)->msp_sensor_flags.sonar)
                    {
                        sc_xbee_api->sendCMD(i, MSP_SONAR_ALTITUDE);
                    }
                    else // if sonar is not available, still wait for 5ms
                    {
                    }
                }
                catch (...)
                {}
                QTime dieTime3 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime3 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }

                // MSP_ATTITUDE
                try
                {
                    sc_xbee_api->sendCMD(i, MSP_ATTITUDE);
                }
                catch (...)
                {}

                QTime dieTime4 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime4 )
                {
                    QEventLoop loop;
                    QTimer::singleShot(1, &loop, SLOT(quit()));
                    loop.exec();
                }

                // MSP_RAW_GPS
                try
                {
                    if (quadstates_list.at(i)->msp_sensor_flags.gps)
                    {
                        sc_xbee_api->sendCMD(i, MSP_RAW_GPS);
                    }
                    else // if gps is not available, still wait for 5ms
                    {
                    }
                }
                catch (...)
                {}
                QTime dieTime5 = QTime::currentTime().addMSecs(30);
                while( QTime::currentTime() < dieTime5 )
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
    {
        QuadStates *tempQS;
        tempQS = quadstates_list.at(0);
        tempQS->mission_list = tempMissionList;
        quadstates_list.replace(0, tempQS);
        sc_xbee_at->uploadMissions();
    }
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
    {
        qDebug() << "Download missions via XBEE AT";
        sc_xbee_at->downloadMissions();
    }
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
        sc_usb->radioControl(radioMode);
    }
    else if (connectionMethod == "AT")
    {
        //qDebug() << "via XBEE AT";
        sc_xbee_at->radioControl(radioMode);
    }
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
    // Record the last time receive state update.
    tReceive.start();
    emit quadsStatesChangeRequest(tempObjList);
}

void SerialCommunication::logMessage(LogMessage tempMessage)
{
    emit logMessageRequest(tempMessage);
}
