#include "serialcommunication_usb.h"

SerialCommunication_USB::SerialCommunication_USB(QObject *parent) :
    QObject(parent)
{
}

SerialCommunication_USB::SerialCommunication_USB(QSerialPort *ser, QList<QuadStates *> tempObjList)
{
    serialportFound = false;
    serial = ser;
    qsList = tempObjList;
    mspHandle1 = new MSP_V1(this);
    mspHandle2 = new MSP_V2(this);
    connect(serial, SIGNAL(readyRead()), this, SLOT(readData()));

    if (serial->open(QIODevice::ReadWrite))
    {
        if(serial->setBaudRate(QSerialPort::Baud115200) &&
                serial->setDataBits(QSerialPort::Data8) &&
                serial->setParity(QSerialPort::NoParity) &&
                serial->setStopBits(QSerialPort::OneStop) &&
                serial->setFlowControl(QSerialPort::NoFlowControl))
        {
            if(serial->isOpen())
            {
                //
                LogMessage tempLogMessage;
                tempLogMessage.id = "SerialCommunication USB";
                tempLogMessage.message = "Connected successfully" + serial->portName();
                emit logMessageRequest(tempLogMessage);
                //
                //qDebug() << "USB: Connected successfully";
                //qDebug() << "USB: Serial Port Name: " << serial->portName();
                serialportFound = true;
            }
        }
    }
    else
    {
        //qDebug() << "USB: Serial Port could not be opened";
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication USB";
        tempLogMessage.message = "Serial Port could not be opened.";
        emit logMessageRequest(tempLogMessage);
    }
}

SerialCommunication_USB::~SerialCommunication_USB()
{
    if(serial->isOpen())
    {
        serial->close();
        //delete mspHandle1;
        //delete mspHandle2;
        //qDebug() << "USB: Serial Port closed successfully";
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication USB";
        tempLogMessage.message = "Serial Port closed successfully.";
        emit logMessageRequest(tempLogMessage);
    }
}

void SerialCommunication_USB::sendCMD(int cmd)  // send cmd with no data
{
    QByteArray output;
    try
    {
        output = mspHandle1->processSendPacket(cmd);
    }
    catch (...)
    {
        //qDebug() << cmd << output;
    }

    try
    {
        send(output);
    }
    catch (...)
    {
        qDebug() << "Send out data unsuccessful";
    }

}

void SerialCommunication_USB::sendCMD(int cmd, Msp_rc_channels raw_rc)  // send rc values
{
    QByteArray output;
    /*
    char checksum = 0;
    output.append("$M<");
    output.append(char(0xFF & 16));
    output.append(char(0xFF & cmd));
    checksum = (char(0xFF & 16)) ^ (char(0xFF & cmd));
    for (int i = 0; i < 8; i++)
    {
        uint16_t tempValue = raw_rc.rcData[i];
        output.append(char(0xFF & tempValue));
        checksum = checksum ^ (char(0xFF & tempValue));
        output.append(char(0xFF & (tempValue >> 8)));
        checksum = checksum ^ (char(0xFF & (tempValue >> 8)));
    }
    // checksum byte
    output.append(checksum );
    */
    output = mspHandle1->processSendPacket(cmd, raw_rc);
    send(output);
}

void SerialCommunication_USB::sendCMD(int cmd, int ind)  // send cmd to check wp info
{
    QByteArray output;
    /*
    output.append("$M<");
    output.append(char(0xFF & 1));
    output.append(char(0xFF & cmd));
    output.append(char(0xFF & ind));
    // checksum byte
    output.append((char(0xFF & 1) ^ char(0xFF & cmd)) ^ char(0xFF & ind) );
    */
    output = mspHandle1->processSendPacket(cmd, ind);
    send(output);
}

void SerialCommunication_USB::sendCMD(int cmd, Mission tempMission) // set mission
{
    QByteArray output;
    /*
    char checksum = 0;
    output.append("$M<");
    output.append(char(0xFF & 21));
    output.append(char(0xFF & cmd));
    checksum = (char(0xFF & 21)) ^ (char(0xFF & cmd));

    output.append(char(0xFF & tempMission.wp_no));
    checksum = checksum ^ (char(0xFF & tempMission.wp_no));

    output.append(char(0xFF & tempMission.wp_action));
    checksum = checksum ^ (char(0xFF & tempMission.wp_action));

    output.append(char(0xFF & tempMission.wp_lat));
    checksum = checksum ^ (char(0xFF & tempMission.wp_lat));
    output.append(char(0xFF & (tempMission.wp_lat >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lat >> 8)));
    output.append(char(0xFF & (tempMission.wp_lat >> 16)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lat >> 16)));
    output.append(char(0xFF & (tempMission.wp_lat >> 24)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lat >> 24)));

    output.append(char(0xFF & tempMission.wp_lon));
    checksum = checksum ^ (char(0xFF & tempMission.wp_lon));
    output.append(char(0xFF & (tempMission.wp_lon >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lon >> 8)));
    output.append(char(0xFF & (tempMission.wp_lon >> 16)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lon >> 16)));
    output.append(char(0xFF & (tempMission.wp_lon >> 24)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_lon >> 24)));

    output.append(char(0xFF & tempMission.wp_alt));
    checksum = checksum ^ (char(0xFF & tempMission.wp_alt));
    output.append(char(0xFF & (tempMission.wp_alt >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_alt >> 8)));
    output.append(char(0xFF & (tempMission.wp_alt >> 16)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_alt >> 16)));
    output.append(char(0xFF & (tempMission.wp_alt >> 24)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_alt >> 24)));

    output.append(char(0xFF & tempMission.wp_p1));
    checksum = checksum ^ (char(0xFF & tempMission.wp_p1));
    output.append(char(0xFF & (tempMission.wp_p1 >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_p1 >> 8)));

    output.append(char(0xFF & tempMission.wp_p2));
    checksum = checksum ^ (char(0xFF & tempMission.wp_p2));
    output.append(char(0xFF & (tempMission.wp_p2 >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_p2 >> 8)));

    output.append(char(0xFF & tempMission.wp_p3));
    checksum = checksum ^ (char(0xFF & tempMission.wp_p3));
    output.append(char(0xFF & (tempMission.wp_p3 >> 8)));
    checksum = checksum ^ (char(0xFF & (tempMission.wp_p3 >> 8)));

    output.append(char(0xFF & tempMission.wp_flag));
    checksum = checksum ^ (char(0xFF & tempMission.wp_flag));

    output.append(checksum);
    */
    output = mspHandle1->processSendPacket(cmd, tempMission);
    send(output);
}

void SerialCommunication_USB::send(QByteArray data)
{
    if(serialportFound && serial->isOpen())
    {
        //qDebug() << "Send: " << data.toHex();
        serial->write(data);
        serial->flush();
    }
    else
    {
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication USB";
        tempLogMessage.message = "Write to Serial Port failed.";
        emit logMessageRequest(tempLogMessage);
        //qDebug() << "USB: Cannot write to Serial Port";
    }
}

void SerialCommunication_USB::readData()
{
    unsigned startDelimiter = 0x24;
    unsigned v1sign = 0x4d;
    unsigned v2sign = 0x58;
    buffer.append(serial->readAll());

    QByteArray packet;

    if (buffer.size() >=1)
    {
        while((unsigned char)buffer.at(0) != (unsigned char)startDelimiter)
        {
            buffer.remove(0, 1);
            if (buffer.size() == 0)
            {
                break;
            }
        }

        if(buffer.size() > 4)
        {
            // MSP V1
            if (buffer.at(1) == (unsigned char)v1sign)
            {
                unsigned length = buffer.at(3)+6;
                if((unsigned char)buffer.size() >= (unsigned char)length){
                    packet.append(buffer.left(length));
                    processPacket(packet);
                    buffer.remove(0, length);
                }
            }
            else
            {}
        }
        if (buffer.size() >= 8)
        {
            // MSP V2
            if (buffer.at(1) == (unsigned char)v2sign)
            {
                uint16_t length = (0xFF & buffer.at(6)) + ((0xFF & buffer.at(7)) << 8) + 8;
                if ((uint16_t)buffer.size() >= length)
                {
                    packet.append(buffer.left(length));
                    //qDebug() << packet.toHex();
                    processPacket(packet);
                    buffer.remove(0, length);
                }
            }
        }
    }
}

void SerialCommunication_USB::processPacket(QByteArray packet)
{
    unsigned v1sign = 0x4d;
    unsigned v2sign = 0x58;
    uint16_t payloadSize = 0;
    uint16_t cmdCode = 0;
    QByteArray data;
    if (packet.at(1) == (unsigned char)v1sign)
    {
        payloadSize = (uint16_t)packet.at(3);
        cmdCode = (uint16_t)packet.at(4);
        data = packet.mid(5, payloadSize+1);
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        QuadStates *resultQS;
        resultQS = mspHandle1->processReceivePacket(packet, tempQS);
        qsList.replace(0, resultQS);
        emit qsReady(&qsList);
    }
    else if (packet.at(1) == (unsigned char)v2sign)
    {
        cmdCode = (0xFF & buffer.at(4)) + ((0xFF & buffer.at(5)) << 8);
        payloadSize = (0xFF & buffer.at(6)) + ((0xFF & buffer.at(7)) << 8);
        data = packet.mid(8, payloadSize+1);
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        QuadStates *resultQS;
        resultQS = mspHandle2->processReceivePacket(packet, tempQS);
        qsList.replace(0, resultQS);
        emit qsReady(&qsList);
    }
    else
    {
        qDebug() << "Unrecognizable MSP format.";
    }
}

void SerialCommunication_USB::parseSensorStatus(QuadStates *tempObj)
{
    uint16_t activeSensors = tempObj->msp_status_ex.packSensorStatus;
    QString tempSensorFlags = QString::number(activeSensors, 2).rightJustified(16, '0');

    if (tempSensorFlags.at(15) == '1')
    {
        tempObj->msp_sensor_flags.acc = true;
    }
    else if (tempSensorFlags.at(15) == '0')
    {
        tempObj->msp_sensor_flags.acc = false;
    }
    if (tempSensorFlags.at(14) == '1')
    {
        tempObj->msp_sensor_flags.baro = true;
    }
    else if (tempSensorFlags.at(14) == '0')
    {
        tempObj->msp_sensor_flags.baro = false;
    }
    if (tempSensorFlags.at(13) == '1')
    {
        tempObj->msp_sensor_flags.mag = true;
    }
    else if (tempSensorFlags.at(13) == '0')
    {
        tempObj->msp_sensor_flags.mag = false;
    }
    if (tempSensorFlags.at(12) == '1')
    {
        tempObj->msp_sensor_flags.gps = true;
    }
    else if (tempSensorFlags.at(12) == '0')
    {
        tempObj->msp_sensor_flags.gps = false;
    }
    if (tempSensorFlags.at(11) == '1')
    {
        tempObj->msp_sensor_flags.sonar = true;
    }
    else if (tempSensorFlags.at(11) == '0')
    {
        tempObj->msp_sensor_flags.sonar = false;
    }
    if (tempSensorFlags.at(9) == '1')
    {
        tempObj->msp_sensor_flags.pitot = true;
    }
    else if (tempSensorFlags.at(9) == '0')
    {
        tempObj->msp_sensor_flags.pitot = false;
    }
    if (tempSensorFlags.at(0) == '1')
    {
        tempObj->msp_sensor_flags.hardware = true;
    }
    else if (tempSensorFlags.at(0) == '0')
    {
        tempObj->msp_sensor_flags.hardware = false;
    }
}

void SerialCommunication_USB::parseArmingFlags(QuadStates *tempObj)
{
    uint16_t armingFlags = tempObj->msp_status_ex.armingFlags;
    QString tempArmingFlags = QString::number(armingFlags, 2).rightJustified(16, '0');

    if (tempArmingFlags.at(15) == '1')
    {
        tempObj->msp_arm_status.OK_TO_ARM = true;
    }
    else if (tempArmingFlags.at(15) == '0')
    {
        tempObj->msp_arm_status.OK_TO_ARM = false;
    }
    if (tempArmingFlags.at(14) == '1')
    {
        tempObj->msp_arm_status.PREVENT_ARMING = true;
    }
    else if (tempArmingFlags.at(14) == '0')
    {
        tempObj->msp_arm_status.PREVENT_ARMING = false;
    }
    if (tempArmingFlags.at(13) == '1')
    {
        tempObj->msp_arm_status.ARMED = true;
    }
    else if (tempArmingFlags.at(13) == '0')
    {
        tempObj->msp_arm_status.ARMED = false;
    }
    if (tempArmingFlags.at(12) == '1')
    {
        tempObj->msp_arm_status.WAS_EVER_ARMED = true;
    }
    else if (tempArmingFlags.at(12) == '0')
    {
        tempObj->msp_arm_status.WAS_EVER_ARMED = false;
    }
    if (tempArmingFlags.at(7) == '1')
    {
        tempObj->msp_arm_status.BLOCK_UAV_NOT_LEVEL = true;
    }
    else if (tempArmingFlags.at(7) == '0')
    {
        tempObj->msp_arm_status.BLOCK_UAV_NOT_LEVEL = false;
    }
    if (tempArmingFlags.at(6) == '1')
    {
        tempObj->msp_arm_status.BLOCK_SENSORS_CALIB = true;
    }
    else if (tempArmingFlags.at(6) == '0')
    {
        tempObj->msp_arm_status.BLOCK_SENSORS_CALIB = false;
    }
    if (tempArmingFlags.at(5) == '1')
    {
        tempObj->msp_arm_status.BLOCK_SYSTEM_OVERLOAD = true;
    }
    else if (tempArmingFlags.at(5) == '0')
    {
        tempObj->msp_arm_status.BLOCK_SYSTEM_OVERLOAD = false;
    }
    if (tempArmingFlags.at(4) == '1')
    {
        tempObj->msp_arm_status.BLOCK_NAV_SAFETY = true;
    }
    else if (tempArmingFlags.at(4) == '0')
    {
        tempObj->msp_arm_status.BLOCK_NAV_SAFETY = false;
    }
    if (tempArmingFlags.at(3) == '1')
    {
        tempObj->msp_arm_status.BLOCK_COMPASS_NOT_CALIB = true;
    }
    else if (tempArmingFlags.at(3) == '0')
    {
        tempObj->msp_arm_status.BLOCK_COMPASS_NOT_CALIB = false;
    }
    if (tempArmingFlags.at(2) == '1')
    {
        tempObj->msp_arm_status.BLOCK_ACC_NOT_CALIB = true;
    }
    else if (tempArmingFlags.at(2) == '0')
    {
        tempObj->msp_arm_status.BLOCK_ACC_NOT_CALIB = false;
    }
    if (tempArmingFlags.at(1) == '1')
    {
        tempObj->msp_arm_status.UNUSED = true;
    }
    else if (tempArmingFlags.at(1) == '0')
    {
        tempObj->msp_arm_status.UNUSED = false;
    }
    if (tempArmingFlags.at(0) == '1')
    {
        tempObj->msp_arm_status.BLOCK_HARDWARE_FAILURE = true;
    }
    else if (tempArmingFlags.at(0) == '0')
    {
        tempObj->msp_arm_status.BLOCK_HARDWARE_FAILURE = false;
    }
}

void SerialCommunication_USB::parseFlightModeFlags(QuadStates *tempObj)
{
    uint32_t flightModeFlags = tempObj->msp_status_ex.packFlightModeFlags;
    QString tempFlightModeFlags = QString::number(flightModeFlags, 2).rightJustified(32, '0');
    for (uint i=0; i< sizeof(tempObj->active_boxids.box_id); i++)
    {
        uint8_t tempInd = tempObj->active_boxids.box_id[i];
        if (tempInd >= 0)
        {
            bool status = false;
            if (tempFlightModeFlags.at(31-i) == '1')
            {
                status = true;
            }
            else if (tempFlightModeFlags.at(31-i) == '0')
            {
                status = false;
            }

            switch (tempInd) {
            case 0: // arm
            {
                tempObj->msp_flight_modes.arm = status;
                break;
            }
            case 1: // angle
            {
                tempObj->msp_flight_modes.angle = status;
                break;
            }
            case 2: // horizon
            {
                tempObj->msp_flight_modes.horizon = status;
                break;
            }
            case 3: // nav althold
            {
                tempObj->msp_flight_modes.nav_althold = status;
                break;
            }
            case 5: // mag
            {
                tempObj->msp_flight_modes.mag = status;
                break;
            }
            case 6: // headfree
            {
                tempObj->msp_flight_modes.headfree = status;
                break;
            }
            case 7: // headadj
            {
                tempObj->msp_flight_modes.headadj = status;
                break;
            }
            case 10: // nav rth
            {
                tempObj->msp_flight_modes.nav_rth = status;
                break;
            }
            case 11: // nav poshold
            {
                tempObj->msp_flight_modes.nav_poshold = status;
                break;
            }
            case 12: // passthru
            {
                tempObj->msp_flight_modes.passthru = status;
                break;
            }
            case 13: // beeper
            {
                tempObj->msp_flight_modes.osd_sw = status;
                break;
            }
            case 19: // osd sw
            {
                tempObj->msp_flight_modes.nav_althold = status;
                break;
            }
            case 26: // black box
            {
                tempObj->msp_flight_modes.blackbox = status;
                break;
            }
            case 27: // failsafe
            {
                tempObj->msp_flight_modes.failsafe = status;
                break;
            }
            case 28: // nav wp
            {
                tempObj->msp_flight_modes.nav_wp = status;
                break;
            }
            case 29: // air mode
            {
                tempObj->msp_flight_modes.air_mode = status;
                break;
            }
            case 30: // home reset
            {
                tempObj->msp_flight_modes.home_reset = status;
                break;
            }
            case 31: // gcs nav
            {
                tempObj->msp_flight_modes.gcs_nav = status;
                break;
            }
            case 32: // heading lock
            {
                tempObj->msp_flight_modes.heading_lock = status;
                break;
            }
            case 33: // surface
            {
                tempObj->msp_flight_modes.surface = status;
                break;
            }
            case 35: // turn assist
            {
                tempObj->msp_flight_modes.turn_assist = status;
                break;
            }
            case 36: // nav launch
            {
                tempObj->msp_flight_modes.nav_launch = status;
                break;
            }
            default:
                break;
            }
        }
    }
}

void SerialCommunication_USB::downloadMission(int id, QuadStates *tempObj)
{
    qDebug() << "Start download mission";
    missionDownloadFlag = false;
    //connect(this, SIGNAL(missionDownloaded()), this, SLOT(missionDownloadedFlag()));
    connect(mspHandle1, &MSP_V1::missionDownloaded, this, &SerialCommunication_USB::missionDownloadedFlag);
    while (!missionDownloadFlag)
    {
        sendCMD(MSP_WP, id);
        qDebug() << "Send mission";
        QTime dieTime= QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime )
        {
            if (missionDownloadFlag)
            {
                break;
            }
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
    }
    qDebug() << "Mission downloaded"
             << tempObj->temp_mission.mi.wp_no
             << tempObj->temp_mission.mi.wp_action
             << tempObj->temp_mission.mi.wp_lat
             << tempObj->temp_mission.mi.wp_lon
             << tempObj->temp_mission.mi.wp_alt
             << tempObj->temp_mission.mi.wp_p1
             << tempObj->temp_mission.mi.wp_p2
             << tempObj->temp_mission.mi.wp_p3
             << tempObj->temp_mission.mi.wp_flag;
}

void SerialCommunication_USB::missionDownloadedFlag()
{
    missionDownloadFlag = true;
}

void SerialCommunication_USB::downloadMissions()
{
    QuadStates *tempQS;
    tempQS = qsList.at(0);
    int ind = 1;
    while(true)
    {
        downloadMission(ind,tempQS);
        if (tempQS->download_mission_list.missions.length() >= ind) // exist
        {
            tempQS->download_mission_list.missions.replace(ind-1,tempQS->temp_mission.mi);
        }
        else if (tempQS->download_mission_list.missions.length() < ind) // not exist
        {
            tempQS->download_mission_list.missions.append(tempQS->temp_mission.mi);
        }

        if (tempQS->temp_mission.mi.wp_flag == 0xa5)
        {
            break;
        }
        else
        {
            ind = ind + 1;
        }
    }
}

void SerialCommunication_USB::uploadMission(Mission tempMission, QuadStates *tempObj)
{
    missionUploadFlag = false;
    while (!missionUploadFlag)
    {
        sendCMD(MSP_SET_WP, tempMission);
        QTime dieTime= QTime::currentTime().addMSecs(100);
        while( QTime::currentTime() < dieTime )
        {
            QEventLoop loop;
            QTimer::singleShot(1, &loop, SLOT(quit()));
            loop.exec();
        }
        downloadMission(tempMission.wp_no,tempObj);
        missionUploadFlag = checkMissionUpload(tempMission, tempObj->temp_mission.mi);
    }
}

void SerialCommunication_USB::missionUploadedFlag()
{}

void SerialCommunication_USB::uploadMissions()
{
    QuadStates *tempObj;
    tempObj = qsList.at(0);
    for (int i = 0; i< tempObj->mission_list.missions.length(); i++)
    {
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication USB";
        tempLogMessage.message = "Upload mission: "
                               + QString::number(tempObj->mission_list.missions.at(i).wp_no, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_action, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_lat, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_lon, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_alt, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_p1, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_p2, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_p3, 10)
                               + QString::number(tempObj->mission_list.missions.at(i).wp_flag, 10);
        emit logMessageRequest(tempLogMessage);
//        qDebug() << "USB - Upload Mission"
//                 << tempObj->mission_list.missions.at(i).wp_no
//                 << tempObj->mission_list.missions.at(i).wp_action
//                 << tempObj->mission_list.missions.at(i).wp_lat
//                 << tempObj->mission_list.missions.at(i).wp_lon
//                 << tempObj->mission_list.missions.at(i).wp_alt
//                 << tempObj->mission_list.missions.at(i).wp_p1
//                 << tempObj->mission_list.missions.at(i).wp_p2
//                 << tempObj->mission_list.missions.at(i).wp_p3
//                 << tempObj->mission_list.missions.at(i).wp_flag;
        uploadMission(tempObj->mission_list.missions.at(i), tempObj);
    }
}

bool SerialCommunication_USB::checkMissionUpload(Mission mi_send, Mission mi_rec)
{
    if ((mi_send.wp_no == mi_rec.wp_no) && (mi_send.wp_action == mi_rec.wp_action)
            && (mi_send.wp_lat == mi_rec.wp_lat) && (mi_send.wp_lon == mi_rec.wp_lon)
            && (mi_send.wp_alt == mi_rec.wp_alt) && (mi_send.wp_p1 == mi_rec.wp_p1)
            && (mi_send.wp_p2 == mi_rec.wp_p2) && (mi_send.wp_p3 == mi_rec.wp_p3)
            && (mi_send.wp_flag == mi_rec.wp_flag))
    {
        //qDebug() << "Same";
        LogMessage tempLogMessage;
        tempLogMessage.id = "SerialCommunication USB";
        tempLogMessage.message = "Same.";
        emit logMessageRequest(tempLogMessage);
        return true;
    }
    else
    {
        return false;
    }
}

// Radio Control is not used with serial port any more.
//    In the future, all the radio control related
//    functions will be moved to RemoteControl class.
//    DataExchange - TelemetrySerialWorker - SerialCommunication - SerialCommunication_USB/XBEE_AT/XBEE_API
//    DataExchange - RemoteControlWorker - RemoteControl_XBEE_AT/XBEE_API
void SerialCommunication_USB::radioControl(int radioMode)
{
    // some radio mode may not be used in this function
    switch (radioMode) {
    case 0:  // 0000 0000 radio is off
    {
        break;
    }
    case 1:  // 0000 0001 radio is on, but arm disarm disnav
    {
        Msp_rc_channels msp_raw_rc;
        msp_raw_rc.rcData[0] = 1500;
        msp_raw_rc.rcData[1] = 1500;
        msp_raw_rc.rcData[2] = 1000;
        msp_raw_rc.rcData[3] = 1500;
        msp_raw_rc.rcData[4] = 1000;
        msp_raw_rc.rcData[5] = 1000;
        msp_raw_rc.rcData[6] = 1000;
        msp_raw_rc.rcData[7] = 1000;
        sendCMD(MSP_SET_RAW_RC, msp_raw_rc);
        break;
    }
    case 3:  // 0000 0011 radio is on, quad1 is armed but disnav
    {
        Msp_rc_channels msp_raw_rc;
        msp_raw_rc.rcData[0] = 1500;
        msp_raw_rc.rcData[1] = 1500;
        msp_raw_rc.rcData[2] = 1000;
        msp_raw_rc.rcData[3] = 1500;
        msp_raw_rc.rcData[4] = 1350;
        msp_raw_rc.rcData[5] = 1000;
        msp_raw_rc.rcData[6] = 1000;
        msp_raw_rc.rcData[7] = 1000;
        sendCMD(MSP_SET_RAW_RC, msp_raw_rc);
        break;
    }
    case 5:  // 0000 0101
    {
        break;
    }
    case 9:  // 0000 1001
    {
        break;
    }
    case 15:  // 0000 1111 radio is on, all quads armed and disnaved
    {
        Msp_rc_channels msp_raw_rc;
        msp_raw_rc.rcData[0] = 1500;
        msp_raw_rc.rcData[1] = 1500;
        msp_raw_rc.rcData[2] = 1000;
        msp_raw_rc.rcData[3] = 1500;
        msp_raw_rc.rcData[4] = 1350;
        msp_raw_rc.rcData[5] = 1000;
        msp_raw_rc.rcData[6] = 1000;
        msp_raw_rc.rcData[7] = 1000;
        sendCMD(MSP_SET_RAW_RC, msp_raw_rc);
        break;
    }
    case 19:  // 0001 0011 radio is on, quad1 is armed and naved
    {
        Msp_rc_channels msp_raw_rc;
        msp_raw_rc.rcData[0] = 1500;
        msp_raw_rc.rcData[1] = 1500;
        msp_raw_rc.rcData[2] = 1000;
        msp_raw_rc.rcData[3] = 1500;
        msp_raw_rc.rcData[4] = 1350;
        msp_raw_rc.rcData[5] = 1000;
        msp_raw_rc.rcData[6] = 1000;
        msp_raw_rc.rcData[7] = 1800;
        sendCMD(MSP_SET_RAW_RC, msp_raw_rc);
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
    case 131:  // 1000 0011 radio is on, quad 1 armed and rth
    {
        Msp_rc_channels msp_raw_rc;
        msp_raw_rc.rcData[0] = 1500;
        msp_raw_rc.rcData[1] = 1500;
        msp_raw_rc.rcData[2] = 1000;
        msp_raw_rc.rcData[3] = 1500;
        msp_raw_rc.rcData[4] = 1350;
        msp_raw_rc.rcData[5] = 1000;
        msp_raw_rc.rcData[6] = 1000;
        msp_raw_rc.rcData[7] = 1600;
        sendCMD(MSP_SET_RAW_RC, msp_raw_rc);
        break;
    }
    default:
    {
        break;
    }
    }
}

void SerialCommunication_USB::arm(QuadStates *tempObj) {}

void SerialCommunication_USB::disarm(QuadStates *tempObj) {}

void SerialCommunication_USB::nav(QuadStates *tempObj) {}

void SerialCommunication_USB::disnav(QuadStates *tempObj) {}
