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
        if (addrList[i] != "")
        {
            rcConnectedDev[i] = true;
        }
        else if (addrList[i] == "")
        {
            rcConnectedDev[i] = false;
        }
    }

    // remote control serial port is opened.
    //    create a new QSerialPort
    rcSerial = new QSerialPort();
    //qDebug() << _serialPortName;
    // To do: the serial port name may changed, need to check
    //    the port name again.
    rcSerial->setPortName(rcSerialPortName);
    if ((rcConnectionName == "USB") || (rcConnectionName == "AT"))
    {
        rc_xbee_at = new RemoteControl_XBEE_AT(rcSerial);
    }
    else if (rcConnectionName == "API")
    {
        rc_xbee_api = new RemoteControl_XBEE_API(rcSerial);
    }


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

//        LogMessage tempLogMessage;
//        tempLogMessage.id = QString("Remote Control");
//        tempLogMessage.message = QString::number(manual_rc_values.rcData[2], 10);
//        emit logMessageRequest(tempLogMessage);
//        //
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
        break;
    }
    case 2:
    {
        for (uint i=0;i<8;i++)
        {
            rc_values[1].rcData[i] = manual_rc_values.rcData[i];
        }
        break;
    }
    case 3:
    {
        for (uint i=0;i<8;i++)
        {
            rc_values[2].rcData[i] = manual_rc_values.rcData[i];
        }
        break;
    }
    default:
        break;
    }
}

void RemoteControl::setValuesFromAuto(uint16_t aMode)
{
    qDebug() << "Auto mode" << aMode;
    for (uint i=0;i<3;i++)
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
    switch (aMode) {
    case 0:    // 000 000 000 000 000 0 radio is off
    {
        break;
    }
    case 1:    // 000 000 000 000 000 1 radio is on, but arm disarm disnav
    {
        break;
    }
    case 2:    // 000 000 000 000 001 0  radio is off, but quad1 is armed. Invalid.
    {
        break;
    }
    case 3:    // 000 000 000 000 001 1 radio is on, quad1 is armed but disnav
    {
        rc_values[0].rcData[4] = 1350;
        break;
    }
    case 4:    // 000 000 000 000 010 0  radio is off, but quad2 is armed. Invalid.
    {
        break;
    }
    case 5:    // 000 000 000 000 010 1  radio is on, quad2 is armed.
    {
        rc_values[1].rcData[4] = 1350;
        break;
    }
    case 6:    // 000 000 000 000 011 0  radio is off, but quad1 and quad 2 are armed. Invalid.
    {
        break;
    }
    case 7:    // 000 000 000 000 011 1  radio is on, quad1 and quad2 are armed.
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

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
    case 8:    // 000 000 000 000 100 0  radio is off, but quad3 is armed. Invalid.
    {
        break;
    }
    case 9:    // 000 000 000 000 100 1  radio is on, quad3 is armed.
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
    case 11:   // 000 000 000 000 101 1 radio is on, quad1 and quad3 armed.
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

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
    case 13:   // 000 000 000 000 110 1 radio is on, quad2 and quad3 armed.
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1000;

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
    case 15:   // 000 000 000 000 111 1 radio is on, all quads armed and disnaved
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
    case 19:   // 000 000 000 001 001 1 radio is on, quad1 is armed and naved
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
    case 23:   // 000 000 000 001 011 1 radio is on, quad1 is armed and naved, quad2 is armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1800;

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
    case 27:   // 000 000 000 001 101 1 radio is on, quad1 is armed and naved, quad3 is armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1800;

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
    case 31:   // 000 000 000 001 111 1 radio is on, quad1 is armed and naved, quad2 and quad3 are armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1800;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1000;

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
    case 37:   // 000 000 000 010 010 1 radio is on, quad2 is armed and naved
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1800;
        break;
    }
    case 39:   // 000 000 000 010 011 1 radio is on, quad2 is armed and naved, quad1 is armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1800;
        break;
    }
    case 45:   // 000 000 000 010 110 1 radio is on, quad2 is armed and naved, quad3 is armed
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1800;

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
    case 47:   // 000 000 000 010 111 1 radio is on, quad2 is armed and naved, quad1 and quad3 are armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1800;

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
    case 73:   // 000 000 000 100 100 1 radio is on, quad3 is armed and naved
    {
        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1800;
        break;
    }
    case 75:   // 000 000 000 100 101 1 radio is on, quad3 is armed and naved, quad1 is armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1800;
        break;
    }
    case 77:   // 000 000 000 100 110 1 radio is on, quad3 is armed and naved, quad2 is armed
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1000;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1800;
        break;
    }
    case 79:   // 000 000 000 100 111 1 radio is on, quad3 is armed and naved, quad1 and quad2 are armed
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1000;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1000;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1800;
        break;
    }
    case 127:  // 000 000 000 111 111 1 radio is on, all quads armed and naved
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
            rc_values[i].rcData[7] = 1800;
        }
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
    case 261:  // 000 000 010 000 010 1 radio is on, quad 1 armed and rth
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
    case 391:  // 000 000 011 000 011 1 radio is on, quad 1 and quad2 armed and rth
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1600;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1600;
        break;
    }
    case 521:  // 000 000 100 000 100 1 radio is on, quad 1 armed and rth
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
    case 651:  // 000 000 101 000 101 1 radio is on, quad 1 and quad 3 armed and rth
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1600;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1600;
        break;
    }
    case 781:  // 000 000 110 000 110 1  radio is on, quad2 and quad3 armed and rth
    {
        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1600;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1600;
    }
    case 911:  // 000 000 111 000 111 1  radio is on, all quads arm and rth. Max value allowed currently.
    {
        rc_values[0].rcData[0] = 1500;
        rc_values[0].rcData[1] = 1500;
        rc_values[0].rcData[2] = 1000;
        rc_values[0].rcData[3] = 1500;
        rc_values[0].rcData[4] = 1350;
        rc_values[0].rcData[5] = 1000;
        rc_values[0].rcData[6] = 1000;
        rc_values[0].rcData[7] = 1600;

        rc_values[1].rcData[0] = 1500;
        rc_values[1].rcData[1] = 1500;
        rc_values[1].rcData[2] = 1000;
        rc_values[1].rcData[3] = 1500;
        rc_values[1].rcData[4] = 1350;
        rc_values[1].rcData[5] = 1000;
        rc_values[1].rcData[6] = 1000;
        rc_values[1].rcData[7] = 1600;

        rc_values[2].rcData[0] = 1500;
        rc_values[2].rcData[1] = 1500;
        rc_values[2].rcData[2] = 1000;
        rc_values[2].rcData[3] = 1500;
        rc_values[2].rcData[4] = 1350;
        rc_values[2].rcData[5] = 1000;
        rc_values[2].rcData[6] = 1000;
        rc_values[2].rcData[7] = 1600;
        break;
    }
    case 1023: // 000 000 111 111 111 1  radio is on, all quads arm, nav, and rth. Invalid.
    {
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
            for (uint i = 0; i < 3; i++)
            {
                if (rcConnectedDev[i] == true)
                {
                    objInd = i;
                    continue;
                }
            }
            rc_xbee_at->sendCMD(MSP_SET_RAW_RC, rc_values[objInd]);
        }
        else if (rcConnectionName == "API")
        {
            // To do: how to deal with xbee address.
            ;
        }
    }
}
