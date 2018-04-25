#include "msp_v2.h"

MSP_V2::MSP_V2()
{

}

QByteArray MSP_V2::processSendPacket(int cmd)
{
    QByteArray output;
    /*
    output.append("$M<");
    output.append(char(0xFF & 0));
    output.append(char(0xFF & cmd));
    output.append((char(0xFF & 0) ^ char(0xFF & cmd)));
    */
    output.append("$X<");
    uint8_t checksum = 0;
    unsigned char flag = char(0xFF & 0);
    output.append(flag); // Flag byte
    checksum = crc8_dvb_s2(checksum, flag);

    unsigned char funcLow = char(0xFF & cmd);
    output.append(funcLow); // Function lsb
    checksum = crc8_dvb_s2(checksum, funcLow);

    unsigned char funcHigh = char(0xFF & (cmd >> 8));
    output.append(funcHigh); // Function hsb
    checksum = crc8_dvb_s2(checksum, funcHigh);

    unsigned char paysizeLow = char(0xFF & 0);
    output.append(paysizeLow); // Payload size lsb
    checksum = crc8_dvb_s2(checksum, paysizeLow);

    unsigned char paysizeHigh = char(0xFF & 0);
    output.append(paysizeHigh); // Payload size hsb
    checksum = crc8_dvb_s2(checksum, paysizeHigh);

    output.append(char(checksum)); // Checksum, crc8_dvb_s2 checksum
    return output;
}
uint8_t MSP_V2::crc8_dvb_s2(uint8_t crc, unsigned char a)
{
    crc ^= a;
    for (int ii = 0; ii < 8; ++ii)
    {
        if (crc & 0x80)
        {
            crc = (crc << 1) ^ 0xD5;
        }
        else
        {
            crc = crc << 1;
        }
    }
    return crc;
}
QuadStates MSP_V2::processReceivePacket(QByteArray packet)
{
    QuadStates *tempQS;
    tempQS = new QuadStates(QByteArray::fromHex("1"),
                            QByteArray::fromHex("0013a20040c14306"),
                            QByteArray::fromHex("fffe"));
    try
    {
        unsigned length = (unsigned char)packet.at(3);
        unsigned cmdCode = (unsigned char)packet.at(4);
        QByteArray data = packet.mid(5, length+1);
        qDebug() << packet.toHex() << length << cmdCode;
        qDebug() << data.toHex() << length << cmdCode;

    switch (cmdCode) {
    case MSP_STATUS_EX:
    {
        if (data.length() < 16)
        {
            qDebug() << "Not right length";
            break;
        }
        try
        {
            qDebug() << "Inside msp status ex";
            uint16_t cycletime = ((0xFF & data.at(1)) << 8) + (0xFF & data.at(0));
            int i2cError = ((0xFF & data.at(3)) << 8) + (0xFF & data.at(2));
            uint16_t activeSensors = ((0xFF & data.at(5)) << 8) + (0xFF & data.at(4));
            uint32_t flightModes = ((0xFF & data.at(9)) << 24) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(7)) << 8) + (0xFF & data.at(6));
            uint8_t configProfile = (0xFF & data.at(10));
            uint16_t systemLoadPercent = ((0xFF & data.at(12)) << 8) + (0xFF & data.at(11));
            uint16_t armingFlags = ((0xFF & data.at(14)) << 8) + (0xFF & data.at(13));
            uint8_t accAxisFlags = (0xFF & data.at(15));


            tempQS->msp_status_ex.cycletime = cycletime;
            tempQS->msp_status_ex.i2cGetErrorCounter = i2cError;
            tempQS->msp_status_ex.packSensorStatus = activeSensors;
            tempQS->msp_status_ex.packFlightModeFlags = flightModes;
            tempQS->msp_status_ex.getConfigProfile = configProfile;
            tempQS->msp_status_ex.averageSystemLoadPercent = systemLoadPercent;
            tempQS->msp_status_ex.armingFlags = armingFlags;
            tempQS->msp_status_ex.accGetCalibrationAxisFlags = accAxisFlags;

            /*QuadStates *tempQS;
            tempQS = qsList.at(0);
            tempQS->msp_status_ex.cycletime = cycletime;
            tempQS->msp_status_ex.i2cGetErrorCounter = i2cError;
            tempQS->msp_status_ex.packSensorStatus = activeSensors;
            tempQS->msp_status_ex.packFlightModeFlags = flightModes;
            tempQS->msp_status_ex.getConfigProfile = configProfile;
            tempQS->msp_status_ex.averageSystemLoadPercent = systemLoadPercent;
            tempQS->msp_status_ex.armingFlags = armingFlags;
            tempQS->msp_status_ex.accGetCalibrationAxisFlags = accAxisFlags;
            parseSensorStatus(tempQS);
            parseFlightModeFlags(tempQS);
            parseArmingFlags(tempQS);
            qsList.replace(0, tempQS);*/
        }
        catch(...)
        {
        }

        break;
    }
    case MSP_STATUS:
    {
        uint16_t cycletime = ((0xFF & data.at(1)) << 8) + (0xFF & data.at(0));
        int i2cError = ((0xFF & data.at(3)) << 8) + (0xFF & data.at(2));
        uint16_t activeSensors = ((0xFF & data.at(5)) << 8) + (0xFF & data.at(4));
        uint32_t flightModes = ((0xFF & data.at(9)) << 24) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(7)) << 8) + (0xFF & data.at(6));
        uint8_t configProfile = (0xFF & data.at(10));

        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_status.cycletime = cycletime;
        tempQS->msp_status.i2cGetErrorCounter = i2cError;
        tempQS->msp_status.packSensorStatus = activeSensors;
        tempQS->msp_status.packFlightModeFlags = flightModes;
        tempQS->msp_status.getConfigProfile = configProfile;

        parseSensorStatus(tempQS);
        parseFlightModeFlags(tempQS);
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_BOXIDS:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);

        for (uint i=0; i< length; i++)
        {
            tempQS->active_boxids.box_id[i] = (0xFF & data.at(i));
            //qDebug() << tempQS->active_boxids.box_id[i];
        }
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_ALTITUDE:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_altitude.estimatedActualPosition = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
        tempQS->msp_altitude.estimatedActualVelocity = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_SONAR_ALTITUDE:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_sonar_altitude.rangefinderGetLatestAltitude = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_ATTITUDE:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_attitude.roll = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
        tempQS->msp_attitude.pitch = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8);
        tempQS->msp_attitude.yaw = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_SET_RAW_RC:
    {
        break;
    }
    case MSP_RC:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        for (int i=0; i<8;i++)
        {
            tempQS->msp_rc_channels.rcData[i] = ((0xFF & data.at(2*i))) + ((0xFF & data.at(2*i+1)) << 8);
        }
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_ANALOG:
    {
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_analog.vbat = (0xFF & data.at(0));
        tempQS->msp_analog.mAhDrawn = (0xFF & data.at(1)) + ((0xFF & data.at(2)) << 8);
        tempQS->msp_analog.rssi = (0xFF & data.at(3)) + ((0xFF & data.at(4)) << 8);
        tempQS->msp_analog.amp = (0xFF & data.at(5)) + ((0xFF & data.at(6)) << 8);
        qsList.replace(0, tempQS);*/
        break;
    }
    case MSP_RAW_IMU:
    {
        QuadStates *tempQS;
        tempQS->msp_raw_imu.acc[0] = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
        tempQS->msp_raw_imu.acc[1] = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8);
        tempQS->msp_raw_imu.acc[2] = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
        tempQS->msp_raw_imu.gyro[0] = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8);
        tempQS->msp_raw_imu.gyro[1] = ((0xFF & data.at(8))) + ((0xFF & data.at(9)) << 8);
        tempQS->msp_raw_imu.gyro[2] = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8);
        tempQS->msp_raw_imu.mag[0] = ((0xFF & data.at(12))) + ((0xFF & data.at(13)) << 8);
        tempQS->msp_raw_imu.mag[1] = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
        tempQS->msp_raw_imu.mag[2] = ((0xFF & data.at(16))) + ((0xFF & data.at(17)) << 8);
        qDebug() << tempQS->msp_raw_imu.acc[0];
        /*QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_raw_imu.acc[0] = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
        tempQS->msp_raw_imu.acc[1] = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8);
        tempQS->msp_raw_imu.acc[2] = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
        tempQS->msp_raw_imu.gyro[0] = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8);
        tempQS->msp_raw_imu.gyro[1] = ((0xFF & data.at(8))) + ((0xFF & data.at(9)) << 8);
        tempQS->msp_raw_imu.gyro[2] = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8);
        tempQS->msp_raw_imu.mag[0] = ((0xFF & data.at(12))) + ((0xFF & data.at(13)) << 8);
        tempQS->msp_raw_imu.mag[1] = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
        tempQS->msp_raw_imu.mag[2] = ((0xFF & data.at(16))) + ((0xFF & data.at(17)) << 8);
        qsList.replace(0, tempQS);*/
        break;
    }
    /*
    case MSP_MOTOR:
    {
        break;
    }
    case MSP_SENSOR_STATUS:
    {
        break;
    }
    case MSP_LOOP_TIME:
    {
        break;
    }
    case MSP_MISC:
    {
        break;
    }
    case MSP_MODE_RANGES:
    {
        break;
    }
    case MSP_RAW_GPS:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_raw_gps.gpsSol_fixType = (0xFF & data.at(0));
        tempQS->msp_raw_gps.gpsSol_numSat = (0xFF & data.at(1));
        tempQS->msp_raw_gps.gpsSol_llh_lat = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8) + ((0xFF & data.at(4)) << 16) + ((0xFF & data.at(5)) << 24);
        tempQS->msp_raw_gps.gpsSol_llh_lon = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
        tempQS->msp_raw_gps.gpsSol_llh_alt = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8);
        tempQS->msp_raw_gps.gpsSol_groundSpeed = ((0xFF & data.at(12))) + ((0xFF & data.at(13)) << 8);
        tempQS->msp_raw_gps.gpsSol_groundCourse = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
        tempQS->msp_raw_gps.gpsSol_hdop = (0xFF & data.at(16));
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_COMP_GPS:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_comp_gps.gps_distanceToHome = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8);
        tempQS->msp_comp_gps.gps_directionToHome = (0xFF & data.at(2)) + + ((0xFF & data.at(3)) << 8);
        tempQS->msp_comp_gps.gpsSol_flags_gpsHeartbeat = (0xFF & data.at(4));
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_NAV_STATUS:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_nav_status.nav_status_mode = (0xFF & data.at(0));
        tempQS->msp_nav_status.nav_status_state = (0xFF & data.at(1));
        tempQS->msp_nav_status.nav_status_activeWPAction = (0xFF & data.at(2));
        tempQS->msp_nav_status.nav_status_activeWPNumber = (0xFF & data.at(3));
        tempQS->msp_nav_status.nav_status_error = (0xFF & data.at(4));
        tempQS->msp_nav_status.MagHoldHeading = (0xFF & data.at(5));
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_GPSSVINFO:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_gps_svinfo.gpsSol_hdop1 = (0xFF & data.at(3));
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_GPSSTATISTICS:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_gps_statistics.gpsStats_lastMessageDt = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8);
        tempQS->msp_gps_statistics.gpsStats_errors = (0xFF & data.at(2)) + ((0xFF & data.at(3)) << 8) + ((0xFF & data.at(4)) << 16) + ((0xFF & data.at(5)) << 24);
        tempQS->msp_gps_statistics.gpsStats_timeouts = (0xFF & data.at(6)) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
        tempQS->msp_gps_statistics.gpsStats_packetCount = (0xFF & data.at(10)) + ((0xFF & data.at(11)) << 8) + ((0xFF & data.at(12)) << 16) + ((0xFF & data.at(13)) << 24);
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_FEATURE:
    {
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->msp_feature.featureMask = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
        qsList.replace(0, tempQS);
        break;
    }
    case MSP_WP:
    {
        Mission tempMission;
        tempMission.wp_no = (0xFF & data.at(0));
        tempMission.wp_action = (0xFF & data.at(1));
        tempMission.wp_lat = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8) + ((0xFF & data.at(4)) << 16) + ((0xFF & data.at(5)) << 24);
        tempMission.wp_lon = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
        tempMission.wp_alt = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8) + ((0xFF & data.at(12)) << 16) + ((0xFF & data.at(13)) << 24);
        tempMission.wp_p1 = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
        tempMission.wp_p2 = ((0xFF & data.at(16))) + ((0xFF & data.at(17)) << 8);
        tempMission.wp_p3 = ((0xFF & data.at(18))) + ((0xFF & data.at(19)) << 8);
        tempMission.wp_flag = (0xFF & data.at(20));
        QuadStates *tempQS;
        tempQS = qsList.at(0);
        tempQS->temp_mission.mi = tempMission;
        qsList.replace(0, tempQS);
        emit missionDownloaded();
        break;
    }*/
    default:
    {
        qDebug() << "Error:  Unknown Packet: " << packet.toHex();
        break;
    }

    }  // end of switch
    }
    catch(...)
    {
        qDebug() << "Get here";
    }
    return *tempQS;
    //emit qsReady(&qsList);
}
