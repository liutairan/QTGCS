#ifndef MSP_V1_H
#define MSP_V1_H

#include <QObject>
#include <QByteArray>
#include <QDebug>
#include "quadstates.h"

//Multiwii MSP Commands
#define MSP_API_VERSION 1
#define MSP_FC_VARIANT 2
#define MSP_FC_VERSION 3
#define MSP_BOARD_INFO 4
#define MSP_BUILD_INFO 5

#define MSP_INAV_PID 6
#define MSP_SET_INAV_PID 7

#define MSP_NAME 10
#define MSP_SET_NAME 11

#define MSP_NAV_POSHOLD 12
#define MSP_SET_NAV_POSHOLD 13

#define MSP_CALIBRATION_DATA 14
#define MSP_SET_CALIBRATION_DATA 15

#define MSP_POSITION_ESTIMATION_CONFIG 16
#define  MSP_SET_POSITION_ESTIMATION_CONFIG 17
#define MSP_WP_MISSION_LOAD 18
#define MSP_WP_MISSION_SAVE 19
#define MSP_WP_GETINFO 20
#define MSP_RTH_AND_LAND_CONFIG 21
#define MSP_SET_RTH_AND_LAND_CONFIG 22
#define MSP_FW_CONFIG 23
#define MSP_SET_FW_CONFIG 24

// MSP commands for Cleanflight original features
#define MSP_CHANNEL_FORWARDING 32
#define MSP_SET_CHANNEL_FORWARDING 33

#define MSP_MODE_RANGES 34    // MSP_MODE_RANGES    out message         Returns all mode ranges
#define MSP_SET_MODE_RANGE 35 // MSP_SET_MODE_RANGE in message          Sets a single mode range

#define MSP_FEATURE 36
#define MSP_SET_FEATURE 37

#define MSP_BOARD_ALIGNMENT 38
#define MSP_SET_BOARD_ALIGNMENT 39

#define MSP_RX_CONFIG = 44
#define MSP_SET_RX_CONFIG = 45

#define MSP_ADJUSTMENT_RANGES 52
#define MSP_SET_ADJUSTMENT_RANGE 53

#define MSP_SONAR_ALTITUDE 58      // SONAR cm

#define MSP_PID_CONTROLLER 59
#define MSP_SET_PID_CONTROLLER 60

#define MSP_ARMING_CONFIG 61         // out message         Returns auto_disarm_delay and disarm_kill_switch parameters
#define MSP_SET_ARMING_CONFIG 62     // in message          Sets auto_disarm_delay and disarm_kill_switch parameters

#define REBOOT 68    // in message reboot settings

#define MSP_LOOP_TIME 73 // out message         Returns FC cycle time i.e looptime parameter
#define MSP_SET_LOOP_TIME 74 // in message          Sets FC cycle time i.e looptime parameter

#define MSP_FAILSAFE_CONFIG 75 // out message         Returns FC Fail-Safe settings
#define MSP_SET_FAILSAFE_CONFIG 76 // in message          Sets FC Fail-Safe settings

#define MSP_RXFAIL_CONFIG 77 // out message         Returns RXFAIL settings
#define MSP_SET_RXFAIL_CONFIG 78 // in message          Sets RXFAIL settings

#define MSP_SENSOR_CONFIG 96
#define MSP_SET_SENSOR_CONFIG 97

#define MSP_IDENT 100
#define MSP_STATUS 101
#define MSP_RAW_IMU 102
#define MSP_SERVO 103
#define MSP_MOTOR 104
#define MSP_RC 105
#define MSP_RAW_GPS 106
#define MSP_COMP_GPS 107
#define MSP_ATTITUDE 108
#define MSP_ALTITUDE 109
#define MSP_ANALOG 110
#define MSP_RC_TUNING 111
#define MSP_PID 112
#define MSP_BOX 113
#define MSP_MISC 114
#define MSP_MOTOR_PINS 115
#define MSP_BOXNAMES 116
#define MSP_PIDNAMES 117
#define MSP_WP 118
#define MSP_BOXIDS 119
#define CONTROL 120
#define MSP_NAV_STATUS 121
#define MSP_NAV_CONFIG 122
#define MSP_3D 124
#define MSP_RC_DEADBAND 125
#define MSP_SENSOR_ALIGNMENT 126

#define MSP_STATUS_EX 150    // out message         cycletime, errors_count, CPU load, sensor present etc
#define MSP_SENSOR_STATUS 151    // out message         Hardware sensor status
#define MSP_UID 160    // out message         Unique device ID

#define MSP_GPSSVINFO 164
#define MSP_GPSSTATISTICS 166

#define RADIO 199
#define MSP_SET_RAW_RC 200
#define MSP_SET_RAW_GPS 201  // in message          fix, numsat, lat, lon, alt, speed
#define SET_PID 202      // in message          P I D coeff (9 are used currently)
#define SET_BOX 203
#define SET_RC_TUNING 204
#define ACC_CALIBRATION 205
#define MAG_CALIBRATION 206
#define MSP_SET_MISC 207
#define RESET_CONF 208
#define MSP_SET_WP 209
#define SELECT_SETTING 210

#define MSP_SET_HEAD 211

#define MSP_SET_SERVO_CONFIGURATION 212
#define MSP_SET_MOTOR 214
#define MSP_SET_NAV_CONFIG 215
#define MSP_SET_3D 217
#define MSP_SET_RC_DEADBAND 218
#define MSP_SET_RESET_CURR_PID 219
#define MSP_SET_SENSOR_ALIGNMENT 220

#define MSP_ACC_TRIM 240    // out message         get acc angle trim values
#define MSP_SET_ACC_TRIM 239    // in message          set acc angle trim values
#define MSP_SERVO_MIX_RULES 241    // out message         Returns servo mixer configuration
#define MSP_SET_SERVO_MIX_RULE 242    // in message          Sets servo mixer configuration
#define MSP_SET_4WAY_IF 245    // in message          Sets 4way interface


#define EEPROM_WRITE 250
#define DEBUG 254

#define INFO_WP 400

class MSP_V1 : public QObject
{
    Q_OBJECT
public:
    explicit MSP_V1(QObject *parent = 0);
    MSP_V1();
    ~MSP_V1();
    QByteArray processSendPacket(int cmd);
    QByteArray processSendPacket(int cmd, Msp_rc_channels raw_rc);
    QByteArray processSendPacket(int cmd, int ind);
    QByteArray processSendPacket(int cmd, Mission tempMission);
    QuadStates* processReceivePacket(QByteArray, QuadStates *);

    void parseSensorStatus(QuadStates *);
    void parseArmingFlags(QuadStates *);
    void parseFlightModeFlags(QuadStates *);
signals:
    void missionDownloaded();
    void missionUploaded();
};

#endif // MSP_V1_H
