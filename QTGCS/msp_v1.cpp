#include "msp_v1.h"

// MSP_V1 is a pack, unpack and parse tool of MSP data flow.
//    It doesn't handle sending or receiving data from
//    serial port, but processes the data going to be
//    sent or received from serial port.
MSP_V1::MSP_V1(QObject *parent) :
    QObject(parent)
{

}

MSP_V1::MSP_V1()
{}

MSP_V1::~MSP_V1()
{}

QByteArray MSP_V1::processSendPacket(int cmd)
{
    QByteArray output;

    output.append("$M<");
    output.append(char(0xFF & 0));
    output.append(char(0xFF & cmd));
    output.append((char(0xFF & 0) ^ char(0xFF & cmd)));

    return output;
}

QByteArray MSP_V1::processSendPacket(int cmd, Msp_rc_channels raw_rc)
{
    QByteArray output;

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
    output.append(checksum);

    return output;
}

QByteArray MSP_V1::processSendPacket(int cmd, int ind)
{
    // This function is used to send out command to request
    //    a special waypoint mission from the FC.
    QByteArray output;

    output.append("$M<");
    output.append(char(0xFF & 1));
    output.append(char(0xFF & cmd));
    output.append(char(0xFF & ind));
    // checksum byte
    output.append((char(0xFF & 1) ^ char(0xFF & cmd)) ^ char(0xFF & ind) );

    return output;
}

QByteArray MSP_V1::processSendPacket(int cmd, Mission tempMission)
{
    // This function is used to send out waypoint mission to FC.
    QByteArray output;

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

    return output;
}

QuadStates* MSP_V1::processReceivePacket(QByteArray packet, QuadStates *tempQS)
{
    try
    {
        unsigned length = (unsigned char)packet.at(3);
        unsigned cmdCode = (unsigned char)packet.at(4);
        QByteArray data = packet.mid(5, length+1);

        switch (cmdCode)
        {
        case MSP_STATUS_EX:
        {
            if (data.length() < 16)
            {
                break;
            }
            try
            {
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

                parseSensorStatus(tempQS);
                //tempQS = parseSensorStatus(tempQS);
                parseFlightModeFlags(tempQS);
                parseArmingFlags(tempQS);
            }
            catch(...)
            {
            }

            break;
        }
        case MSP_STATUS:
        {
            uint16_t cycletime = ((0xFF & data.at(1)) << 8) + (0xFF & data.at(0));
            uint16_t i2cError = ((0xFF & data.at(3)) << 8) + (0xFF & data.at(2));
            uint16_t activeSensors = ((0xFF & data.at(5)) << 8) + (0xFF & data.at(4));
            uint32_t flightModes = ((0xFF & data.at(9)) << 24) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(7)) << 8) + (0xFF & data.at(6));
            uint8_t configProfile = (0xFF & data.at(10));

            tempQS->msp_status.cycletime = cycletime;
            tempQS->msp_status.i2cGetErrorCounter = i2cError;
            tempQS->msp_status.packSensorStatus = activeSensors;
            tempQS->msp_status.packFlightModeFlags = flightModes;
            tempQS->msp_status.getConfigProfile = configProfile;

            parseSensorStatus(tempQS);
            //tempQS = parseSensorStatus(tempQS);
            parseFlightModeFlags(tempQS);
            break;
        }
        case MSP_IDENT:
        {
            uint8_t version = ((0xFF & data.at(0))/100.0);
            uint8_t multiType = (0xFF & data.at(1));
            uint8_t msp_version = (0xFF & data.at(2));
            uint32_t capability = ((0xFF & data.at(6)) << 24) + ((0xFF & data.at(5)) << 16) + ((0xFF & data.at(4)) << 8) + (0xFF & data.at(3));
            tempQS->msp_ident.MW_VERSION = version;
            tempQS->msp_ident.mixer_mode = multiType;
            tempQS->msp_ident.MSP_PROTOCOL_VERSION = msp_version;
            tempQS->msp_ident.CAPABILITY = capability;
            //qDebug() << version << multiType << msp_version << capability;
            break;
        }
        case MSP_BOXIDS:
        {
            for (uint i=0; i< length; i++)
            {
                tempQS->active_boxids.box_id[i] = (0xFF & data.at(i));
            }
            break;
        }
        case MSP_ALTITUDE:
        {
            tempQS->msp_altitude.estimatedActualPosition = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
            tempQS->msp_altitude.estimatedActualVelocity = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
            // Barometer
            tempQS->msp_altitude.baroAltitude = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
            break;
        }
        case MSP_SONAR_ALTITUDE:
        {
            tempQS->msp_sonar_altitude.rangefinderGetLatestAltitude = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
            break;
        }
        case MSP_ATTITUDE:
        {
            tempQS->msp_attitude.roll = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
            tempQS->msp_attitude.pitch = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8);
            tempQS->msp_attitude.yaw = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
            break;
        }
        case MSP_SET_RAW_RC:
        {
            break;
        }
        case MSP_RC:
        {
            for (int i=0; i<8;i++)
            {
                tempQS->msp_rc_channels.rcData[i] = ((0xFF & data.at(2*i))) + ((0xFF & data.at(2*i+1)) << 8);
            }
            break;
        }
        case MSP_RC_TUNING:
        {
            break;
        }
        case MSP_ANALOG:
        {
            tempQS->msp_analog.vbat = (0xFF & data.at(0));
            tempQS->msp_analog.mAhDrawn = (0xFF & data.at(1)) + ((0xFF & data.at(2)) << 8);
            tempQS->msp_analog.rssi = (0xFF & data.at(3)) + ((0xFF & data.at(4)) << 8);
            tempQS->msp_analog.amp = (0xFF & data.at(5)) + ((0xFF & data.at(6)) << 8);
            break;
        }
        case MSP_RAW_IMU:
        {
            tempQS->msp_raw_imu.acc[0] = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
            tempQS->msp_raw_imu.acc[1] = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8);
            tempQS->msp_raw_imu.acc[2] = ((0xFF & data.at(4))) + ((0xFF & data.at(5)) << 8);
            tempQS->msp_raw_imu.gyro[0] = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8);
            tempQS->msp_raw_imu.gyro[1] = ((0xFF & data.at(8))) + ((0xFF & data.at(9)) << 8);
            tempQS->msp_raw_imu.gyro[2] = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8);
            tempQS->msp_raw_imu.mag[0] = ((0xFF & data.at(12))) + ((0xFF & data.at(13)) << 8);
            tempQS->msp_raw_imu.mag[1] = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
            tempQS->msp_raw_imu.mag[2] = ((0xFF & data.at(16))) + ((0xFF & data.at(17)) << 8);
            break;
        }
        case MSP_MOTOR:
        {
            break;
        }
        case MSP_SENSOR_STATUS:
        {
            uint8_t isHardwareHealthy = (0xFF & data.at(0));
            uint8_t gyroHwStatus = (0xFF & data.at(1));
            uint8_t accHwStatus = (0xFF & data.at(2));
            uint8_t magHwStatus = (0xFF & data.at(3));
            uint8_t baroHwStatus = (0xFF & data.at(4));
            uint8_t gpsHwStatus = (0xFF & data.at(5));
            uint8_t rangeHwStatus = (0xFF & data.at(6));
            uint8_t speedHwStatus = (0xFF & data.at(7));
            uint8_t flowHwStatus = (0xFF & data.at(8));
            // To do, pass value to QS
            break;
        }
        case MSP_LOOP_TIME:
        {
            tempQS->msp_loop_time.looptime = ((0xFF & data.at(0))) + ((0xFF & data.at(1)) << 8);
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
            tempQS->msp_raw_gps.gpsSol_fixType = (0xFF & data.at(0));
            tempQS->msp_raw_gps.gpsSol_numSat = (0xFF & data.at(1));
            tempQS->msp_raw_gps.gpsSol_llh_lat = ((0xFF & data.at(2))) + ((0xFF & data.at(3)) << 8) + ((0xFF & data.at(4)) << 16) + ((0xFF & data.at(5)) << 24);
            tempQS->msp_raw_gps.gpsSol_llh_lon = ((0xFF & data.at(6))) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
            tempQS->msp_raw_gps.gpsSol_llh_alt = ((0xFF & data.at(10))) + ((0xFF & data.at(11)) << 8);
            tempQS->msp_raw_gps.gpsSol_groundSpeed = ((0xFF & data.at(12))) + ((0xFF & data.at(13)) << 8);
            tempQS->msp_raw_gps.gpsSol_groundCourse = ((0xFF & data.at(14))) + ((0xFF & data.at(15)) << 8);
            tempQS->msp_raw_gps.gpsSol_hdop = (0xFF & data.at(16));
            break;
        }
        case MSP_COMP_GPS:
        {
            tempQS->msp_comp_gps.gps_distanceToHome = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8);
            tempQS->msp_comp_gps.gps_directionToHome = (0xFF & data.at(2)) + + ((0xFF & data.at(3)) << 8);
            tempQS->msp_comp_gps.gpsSol_flags_gpsHeartbeat = (0xFF & data.at(4));
            break;
        }
        case MSP_NAV_STATUS:
        {
            tempQS->msp_nav_status.nav_status_mode = (0xFF & data.at(0));
            tempQS->msp_nav_status.nav_status_state = (0xFF & data.at(1));
            tempQS->msp_nav_status.nav_status_activeWPAction = (0xFF & data.at(2));
            tempQS->msp_nav_status.nav_status_activeWPNumber = (0xFF & data.at(3));
            tempQS->msp_nav_status.nav_status_error = (0xFF & data.at(4));
            tempQS->msp_nav_status.MagHoldHeading = (0xFF & data.at(5));
            break;
        }
        case MSP_GPSSVINFO:
        {
            tempQS->msp_gps_svinfo.gpsSol_hdop1 = (0xFF & data.at(3));
            break;
        }
        case MSP_GPSSTATISTICS:
        {
            tempQS->msp_gps_statistics.gpsStats_lastMessageDt = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8);
            tempQS->msp_gps_statistics.gpsStats_errors = (0xFF & data.at(2)) + ((0xFF & data.at(3)) << 8) + ((0xFF & data.at(4)) << 16) + ((0xFF & data.at(5)) << 24);
            tempQS->msp_gps_statistics.gpsStats_timeouts = (0xFF & data.at(6)) + ((0xFF & data.at(7)) << 8) + ((0xFF & data.at(8)) << 16) + ((0xFF & data.at(9)) << 24);
            tempQS->msp_gps_statistics.gpsStats_packetCount = (0xFF & data.at(10)) + ((0xFF & data.at(11)) << 8) + ((0xFF & data.at(12)) << 16) + ((0xFF & data.at(13)) << 24);
            tempQS->msp_gps_statistics.gpsSol_hdop = (0xFF & data.at(14)) + ((0xFF & data.at(15)) << 8);
            tempQS->msp_gps_statistics.gpsSol_eph = (0xFF & data.at(16)) + ((0xFF & data.at(17)) << 8);
            tempQS->msp_gps_statistics.gpsSol_epv = (0xFF & data.at(18)) + ((0xFF & data.at(19)) << 8);
            break;
        }
        case MSP_FEATURE:
        {
            tempQS->msp_feature.featureMask = (0xFF & data.at(0)) + ((0xFF & data.at(1)) << 8) + ((0xFF & data.at(2)) << 16) + ((0xFF & data.at(3)) << 24);
            break;
        }
        case MSP_PID:
        {
            break;
        }
        case MSP_ARMING_CONFIG:
        {
            break;
        }
        case MSP_3D:
        {
            break;
        }
        case MSP_SET_3D:
        {
            break;
        }
        case MSP_MOTOR_PINS:
        {
            break;
        }
        case MSP_BOXNAMES:
        {
            break;
        }
        case MSP_PIDNAMES:
        {
            break;
        }
        case MSP_SERVO_MIX_RULES:
        {
            break;
        }
        case MSP_SET_SERVO_MIX_RULE:
        {
            break;
        }
        case MSP_SET_SERVO_CONFIGURATION:
        {
            break;
        }
        case MSP_RC_DEADBAND:
        {
            break;
        }
        case MSP_SET_RC_DEADBAND:
        {
            break;
        }
        case MSP_SENSOR_ALIGNMENT:
        {
            break;
        }
        case MSP_SET_RAW_GPS:
        {
            break;
        }
        case MSP_SET_PID_CONTROLLER:
        {
            break;
        }
        case MSP_SET_MISC:
        {
            break;
        }
        case MSP_API_VERSION:
        {
            break;
        }
        case MSP_FC_VARIANT:
        {
            break;
        }
        case MSP_FC_VERSION:
        {
            break;
        }
        case MSP_BUILD_INFO:
        {
            break;
        }
        case MSP_BOARD_INFO:
        {
            break;
        }
        case MSP_ADJUSTMENT_RANGES:
        {
            break;
        }
        case MSP_FAILSAFE_CONFIG:
        {
            break;
        }
        case MSP_SENSOR_CONFIG:
        {
            break;
        }
        case MSP_INAV_PID:
        {
            break;
        }
        case MSP_SET_INAV_PID:
        {
            break;
        }
        case MSP_NAV_POSHOLD:
        {
            break;
        }
        case MSP_SET_NAV_POSHOLD:
        {
            break;
        }
        case MSP_RTH_AND_LAND_CONFIG:
        {
            break;
        }
        case MSP_SET_RTH_AND_LAND_CONFIG:
        {
            break;
        }
        case MSP_NAME:
        {
            break;
        }
        case MSP_SET_NAME:
        {
            break;
        }
        case MSP_WP_GETINFO:
        {
            break;
        }
        case MSP_SET_WP:
        {
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

            tempQS->temp_mission.mi = tempMission;
            emit missionDownloaded();
            break;
        }
        case MSP_WP_MISSION_SAVE:
        {
            break;
        }
        case MSP_WP_MISSION_LOAD:
        {
            break;
        }
        default:
        {
            qDebug() << "Error:  Unknown Packet: " << packet.toHex();
            break;
        }

        }  // end of switch
    }
    catch(...)
    {
    }

    return tempQS;
}

void MSP_V1::parseSensorStatus(QuadStates *tempObj)
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
    //return tempObj;
}

void MSP_V1::parseArmingFlags(QuadStates *tempObj)
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

void MSP_V1::parseFlightModeFlags(QuadStates *tempObj)
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
