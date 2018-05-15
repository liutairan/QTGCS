#include "remotecontrol.h"

RemoteControl::RemoteControl(QObject *parent) : QObject(parent)
{

}

RemoteControl::RemoteControl(QString portName, QString connectionName, QString addrList[], QObject *parent) :
    QObject(parent)
{
    rcSerialPortName = portName;
    rcConnectionName = connectionName;
    for (uint i=0;i<3;i++)
    {
        rcAddrList[i] = addrList[i];
    }

    // remote control serial port is opened.
    //    create a new QSerialPort
    rcSerial = new QSerialPort();
    //qDebug() << _serialPortName;
    // To do: the serial port name may changed, need to check
    //    the port name again.
    rcSerial->setPortName(rcSerialPortName);
    rc_xbee_at = new RemoteControl_XBEE_AT(rcSerial);

    // serial remote control output
    rcTimer = new QTimer();
    QObject::connect(rcTimer, SIGNAL(timeout()), this, SLOT(rcSwitch()));
    // Time specified in ms, as the frequency to send out remote control
    //    command to one agent for manual control.
    rcTimer->start(100);
    rcSerialReady = true;

    manualMode = 0;
    autoMode = 0;
    initRCValues();
}

void RemoteControl::stop()
{
    if (rcSerial->isOpen())
    {
        rcSerial->close();
    }
    delete rcSerial;

    rcSerialReady = false;
    rcTimer->stop();
}

void RemoteControl::initRCValues()
{
    // Even though 8 channels are set here, there is still a possibility
    //    more channels are used. This is depending on the settings.
    //    Traditionally, at most 8 channels were available on the remote
    //    controller. But recently more channels are available.
    //    For example, iNav support 18 channels.
    manual_rc_values.rcData[0] = 1500;
    manual_rc_values.rcData[1] = 1500;
    manual_rc_values.rcData[2] = 1000;
    manual_rc_values.rcData[3] = 1500;
    manual_rc_values.rcData[4] = 1000;
    manual_rc_values.rcData[5] = 1000;
    manual_rc_values.rcData[6] = 1000;
    manual_rc_values.rcData[7] = 1000;

    for (uint i=0; i<3; i++)
    {
        rc_values[i].rcData[0] = 1500;
        rc_values[i].rcData[1] = 1500;
        rc_values[i].rcData[2] = 1000;
        rc_values[i].rcData[3] = 1500;
        rc_values[i].rcData[4] = 1000;
        rc_values[i].rcData[5] = 1000;
        rc_values[i].rcData[6] = 1000;
        rc_values[i].rcData[7] = 1000;
    }
}

void RemoteControl::updateRCValues(QString msg)
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

uint16_t RemoteControl::mapAngleToPWM(float realAngle, float minAngle, float maxAngle, uint16_t minPWM, uint16_t maxPWM)
{
    uint16_t realPWM = int((maxPWM - minPWM)*(realAngle - minAngle)/(maxAngle - minAngle) + minPWM);
    return realPWM;
}

void RemoteControl::rcSwitch()
{
    if (rcSerialReady)
    {
        rcWorker();
    }
}

void RemoteControl::rcWorker()
{
    // First, use rcMode to set up rc output
    // Second, use manualMode to overlap with manual control command.

    if (rcSerialReady == true)
    {
        // Set RC values from auto settings: ARM/NAV/RTH
        setValuesFromAuto(autoMode);
        // Set RC values from manual input,
        //    overlapping previous auto settings
        //    if both modes are available for that agent.
        setValuesFromManual(manualMode);
        // Send out command via XBee, AT or API.
        sendCommand();
    }
}

void RemoteControl::setValuesFromManual(uint8_t mMode)
{
    //qDebug() << "Manual mode" << mMode;
    switch (mMode) {
    case 0:
    {
        break;
    }
    case 1:
    {
        for (uint i=0;i<8;i++)
        {
            rc_values[0].rcData[i] = manual_rc_values.rcData[i];
        }

        /*qDebug() << manual_rc_values.rcData[0]
                 << manual_rc_values.rcData[1]
                 << manual_rc_values.rcData[2]
                 << manual_rc_values.rcData[3]
                 << manual_rc_values.rcData[4]
                 << manual_rc_values.rcData[5]
                 << manual_rc_values.rcData[6]
                 << manual_rc_values.rcData[7];*/
        // Send log info to main GUI
        /*LogMessage tempLogMessage;
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

        tempLogMessage.id = QString("Remote Control");
        tempLogMessage.message = "RC mode " + QString::number(rcMode, 10);
        emit logMessageRequest(tempLogMessage);*/
        //
        //rc_xbee_at = new RemoteControl_XBEE_AT(serial);
        //rc_xbee_at->sendCMD(MSP_SET_RAW_RC, manual_rc_values);
        break;
    }
    case 2:
    {
        for (uint i=0;i<8;i++)
        {
            rc_values[1].rcData[i] = manual_rc_values.rcData[i];
        }
        // Send log info to main GUI
//        LogMessage tempLogMessage;
//        tempLogMessage.id = QString("Remote Control");
//        tempLogMessage.message = QString("RC is not set on CH2.");
//        emit logMessageRequest(tempLogMessage);
        //
        //qDebug() << "Not set 2";
        break;
    }
    case 3:
    {
        for (uint i=0;i<8;i++)
        {
            rc_values[2].rcData[i] = manual_rc_values.rcData[i];
        }
        // Send log info to main GUI
//        LogMessage tempLogMessage;
//        tempLogMessage.id = QString("Remote Control");
//        tempLogMessage.message = QString("RC is not set on CH3.");
//        emit logMessageRequest(tempLogMessage);
        //
        //qDebug() << "Not set 3";
        break;
    }
    default:
        break;
    }
}

void RemoteControl::setValuesFromAuto(uint16_t aMode)
{
    //qDebug() << "Auto mode" << aMode;
    switch (aMode) {
    case 0:  // 0000 0000 radio is off
    {
        break;
    }
    case 1:  // 000 000 000 000 000 1 radio is on, but arm disarm disnav
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1000;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;
        break;
    }
    case 3:  // 000 000 000 000 001 1 radio is on, quad1 is armed but disnav
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;
        break;
    }
    case 5:  // 000 000 000 000 010 1
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1000;
        break;
    }
    case 9:  // 000 000 000 000 100 1
    {
        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1000;
        break;
    }
    case 15:  // 000 000 000 000 111 1 radio is on, all quads armed and disnaved
    {
        for (uint i=0; i<3; i++)
        {
            rc_values[i].rcData[0] = 1500;
            rc_values[i].rcData[1] = 1500;
            rc_values[i].rcData[2] = 1000;
            rc_values[i].rcData[3] = 1500;
            rc_values[i].rcData[4] = 1350;
            rc_values[i].rcData[5] = 1000;
            rc_values[i].rcData[6] = 1000;
            rc_values[i].rcData[7] = 1000;
        }
        break;
    }
    case 19:  // 000 000 000 001 001 1 radio is on, quad1 is armed and naved
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1800;
        break;
    }
    case 37:  // 0010 0101
    {
        break;
    }
    case 81:  // 0100 1001
    {
        break;
    }
    case 127:  // 0111 1111 radio is on, all quads armed and naved
    {
        break;
    }
    case 131:  // 000 000 001 000 001 1 radio is on, quad 1 armed and rth
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1600;
        break;
    }
    default:
    {
        break;
    }
    }
}

void RemoteControl::sendCommand()
{
    if ( (manualMode>0) || (autoMode>0) )
    {
        if ((rcConnectionName == "USB") || (rcConnectionName == "AT"))
        {
            // USB or AT mode, should only have one connection
            //    Find the first connection and send out command
            uint8_t objInd = 0;
            for (uint i=0;i<3;i++)
            {
                if (rcAddrList[i] != "")
                {
                    objInd = i;
                    continue;
                }
            }
            rc_xbee_at->sendCMD(MSP_SET_RAW_RC, rc_values[objInd]);
        }
        else if (rcConnectionName == "API")
        {}
    }
}
