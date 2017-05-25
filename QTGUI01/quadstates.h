#ifndef QUADSTATES_H
#define QUADSTATES_H

#include <QtGlobal>
#include <QList>

/* Defined in src/main/fc/fc_msp.c */

// board info
struct Msp_api_version
{
    quint8 MSP_PROTOCOL_VERSION;
    quint8 API_VERSION_MAJOR;
    quint8 API_VERSION_MINOR;
};

struct Msp_fc_variant
{
    QByteArray *fc_id;
};

struct Msp_fc_version
{
    quint8 FC_VERSION_MAJOR;
    quint8 FC_VERSION_MINOR;
    quint8 FC_VERSION_PATCH_LEVEL;
};

struct Msp_board_info
{
    QByteArray *bd_id;
    quint16 hardware_revision;
    quint16 place_holder;
};

struct Msp_ident
{
    quint8 MW_VERSION;
    quint8 mixer_mode;
    quint8 MSP_PROTOCOL_VERSION;
    quint32 CAPABILITY;
};

struct Msp_misc
{
    quint16 midrc;
    quint16 minthrottle;
    quint16 maxthrottle;
    quint16 mincommand;
    quint16 failsafe_throttle;
    quint8 provider;
    quint8 gps_baudrate;
    quint8 sbasMode;

    quint8 multiwiiCurrentMeterOutput;
    quint8 rssi_channels;
    quint8 place_holder;

    quint16 mag_declination;
    quint8 vbatscale;
    quint8 vbatmincellvoltage;
    quint8 vbatmaxcellvoltage;
    quint8 vbatwarningcellvoltage;
};

struct Msp_motor_pins
{
    uint8_t pin[8];
};

struct Msp_status
{
    uint16_t cycletime;
    uint16_t i2cGetErrorCounter;
    uint16_t packSensorStatus;
    uint32_t packFlightModeFlags;
    uint8_t getConfigProfile;
};

struct Msp_status_ex
{
    uint16_t cycletime;
    uint16_t i2cGetErrorCounter;
    uint16_t packSensorStatus;
    uint32_t packFlightModeFlags;
    uint8_t getConfigProfile;
    uint16_t averageSystemLoadPercent;
    uint16_t armingFlags;
    uint8_t accGetCalibrationAxisFlags;
};

struct Msp_loop_time
{
    quint16 looptime;
};

//struct Msp_active_boxes
//{};

struct Msp_mode_ranges  // need to adjust
{
    quint8 box_id;
    quint8 auxChannelIndex;
    quint8 startStep;
    quint8 endStep;
};

struct Msp_adjustment_ranges
{};

struct Msp_boxnames
{};

struct Msp_boxids  // need to adjust based on length
{
    uint8_t box_id[32];
};

struct Msp_flight_modes
{
    bool arm;             // 0
    bool angle;           // 1
    bool horizon;         // 2
    bool nav_althold;     // 3
    bool mag;             // 5
    bool headfree;        // 6
    bool headadj;         // 7
    bool camstab;         // 8
    bool nav_rth;         // 10
    bool nav_poshold;     // 11
    bool passthru;        // 12
    bool beeper;          // 13
    bool osd_sw;          // 19
    bool telemetry;       // 20
    bool blackbox;        // 26
    bool failsafe;        // 27
    bool nav_wp;          // 28
    bool air_mode;        // 29
    bool home_reset;      // 30
    bool gcs_nav;         // 31
    bool heading_lock;    // 32
    bool surface;         // 33
    bool turn_assist;     // 35
    bool nav_launch;      // 36
};

struct Msp_arming_config
{
    uint8_t auto_disarm_delay;
    uint8_t disarm_kill_switch;
};

struct Msp_arm_status
{
    bool OK_TO_ARM;
    bool PREVENT_ARMING;
    bool ARMED;
    bool WAS_EVER_ARMED;
    bool BLOCK_UAV_NOT_LEVEL;
    bool BLOCK_SENSORS_CALIB;
    bool BLOCK_SYSTEM_OVERLOAD;
    bool BLOCK_NAV_SAFETY;
    bool BLOCK_COMPASS_NOT_CALIB;
    bool BLOCK_ACC_NOT_CALIB;
    bool UNUSED;
    bool BLOCK_HARDWARE_FAILURE;
};

struct Msp_debug
{};

struct Msp_uid
{
    uint32_t u_id_0;
    uint32_t u_id_1;
    uint32_t u_id_2;
};

struct Msp_feature
{
    uint32_t featureMask;
};

struct Msp_board_alignment
{
    uint16_t rollDeciDegrees;
    uint16_t pitchDeciDegrees;
    uint16_t yawDeciDegrees;
};

struct Msp_voltage_meter_config
{};

struct Msp_current_meter_config
{};

struct Msp_mixer
{};

struct Msp_rx_config
{};

struct Msp_failsafe_config
{};

struct Msp_rssi_config
{};

struct Msp_rx_map
{};

struct Msp_rc_deadband
{};

struct Msp_sensor_alignment
{};

struct Msp_advanced_config
{};

// sensor
struct Msp_sensor_status
{
    uint8_t HardwareHealthy;
    uint8_t HWGyroStatus;
    uint8_t HWAcceStatus;
    uint8_t HWCompStatus;
    uint8_t HWBaroStatus;
    uint8_t HWGPSStatus;
    uint8_t HWRangefinderStatus;
    uint8_t HWPitotmeterStatus;
    uint8_t HW_SENSOR_NONE;
};

struct Msp_sensor_flags
{
    bool hardware;
    bool pitot;
    bool sonar;
    bool gps;
    bool mag;
    bool baro;
    bool acc;
};

struct Msp_altitude
{
    uint32_t estimatedActualPosition;
    uint16_t estimatedActualVelocity;
};

struct Msp_sonar_altitude
{
    uint32_t rangefinderGetLatestAltitude;
};

struct Msp_raw_imu
{
    int16_t acc[3];
    int16_t gyro[3];
    int16_t mag[3];
};

struct Msp_attitude
{
    int16_t roll;
    int16_t pitch;
    int16_t yaw;
};

struct Msp_analog
{
    uint8_t vbat;
    uint16_t mAhDrawn;
    uint16_t rssi;
    uint16_t amp;
};

struct Msp_motor
{
    uint16_t motor[8];
};

// PID
struct Msp_pid
{};

struct Msp_pidnames
{};

struct Msp_pid_controller
{};

// wireless
struct Msp_radio
{};

struct Msp_rc_tuning
{
    quint8 space_holder;
    quint8 rcExpo8;
    quint8 rates[3];
    quint8 dynThrPID;
    quint8 thrMid8;
    quint8 thrExpo8;
    quint16 tpa_breakpoint;
    quint8 rcYawExpo8;
};

struct Msp_rc_channels
{
    uint16_t rcData[8];
};

// GPS and NAV
struct Msp_raw_gps
{
    uint8_t gpsSol_fixType;
    uint8_t gpsSol_numSat;
    int32_t gpsSol_llh_lat;
    int32_t gpsSol_llh_lon;
    int16_t gpsSol_llh_alt; // unit: meters
    int16_t gpsSol_groundSpeed;
    int16_t gpsSol_groundCourse;
    int16_t gpsSol_hdop;
};

struct Msp_comp_gps
{
    uint16_t gps_distanceToHome;
    uint16_t gps_directionToHome;
    uint8_t gpsSol_flags_gpsHeartbeat;
};

struct Msp_gps_svinfo
{
    uint8_t space_holder1;
    uint8_t space_holder2;
    uint8_t space_holder3;
    uint8_t gpsSol_hdop1;
    uint8_t gpsSol_hdop2;
};

struct Msp_gps_statistics
{
    uint16_t gpsStats_lastMessageDt;
    uint32_t gpsStats_errors;
    uint32_t gpsStats_timeouts;
    uint32_t gpsStats_packetCount;
    uint16_t gpsSol_hdop;
    uint16_t gpsSol_eph;
    uint16_t gpsSol_epv;
};

struct WP
{
    uint8_t wp_no;
    QString wp_action;
    double wp_lat;
    double wp_lon;
    double wp_alt; // unit: cm
    int16_t wp_p1;
    int16_t wp_p2;
    int16_t wp_p3;
    uint8_t wp_flag;
};

struct WP_list
{
    QList<WP> wps;
};

struct Msp_wp
{
    uint8_t wp_no;
    uint8_t wp_action;
    int32_t wp_lat;
    int32_t wp_lon;
    int32_t wp_alt; // unit: cm
    int16_t wp_p1;
    int16_t wp_p2;
    int16_t wp_p3;
    uint8_t wp_flag;
};

struct Msp_nav_status
{
    uint8_t nav_status_mode;
    uint8_t nav_status_state;
    uint8_t nav_status_activeWPAction;
    uint8_t nav_status_activeWPNumber;
    uint8_t nav_status_error;
    uint16_t MagHoldHeading;
};

struct Msp_nav_config
{};

struct Msp_nav_poshold
{
    uint8_t user_control_mode;
    uint16_t max_speed;
    uint16_t max_climb_rate;
    uint16_t max_manual_speed;
    uint16_t max_manual_climb_rate;
    uint8_t max_bank_angle;
    uint8_t use_thr_mid_for_althold;
    uint16_t hover_throttle;
};

// missions
struct Mission
{
    uint8_t wp_no;
    uint8_t wp_action;
    int32_t wp_lat;
    int32_t wp_lon;
    int32_t wp_alt; // unit: cm
    int16_t wp_p1;
    int16_t wp_p2;
    int16_t wp_p3;
    uint8_t wp_flag;
};

struct Temp_mission
{
    Mission mi;
};

struct Mission_list
{
    QList<Mission> missions;
};

struct Download_mission_list
{
    QList<Mission> missions;
};

class QuadStates
{
public:
    QuadStates(QByteArray, QByteArray, QByteArray);
    QByteArray frame_id;
    QByteArray address_long;
    QByteArray address_short;

    Msp_boxids active_boxids;
    Msp_analog msp_analog;
    Msp_status msp_status;
    Msp_status_ex msp_status_ex;
    Msp_sensor_flags msp_sensor_flags;
    Msp_arm_status msp_arm_status;
    Msp_flight_modes msp_flight_modes;
    Msp_rc_channels msp_rc_channels;
    Msp_feature msp_feature;
    Msp_raw_imu msp_raw_imu;
    Temp_mission temp_mission;
    Mission_list mission_list;
    Download_mission_list download_mission_list;
    Msp_altitude msp_altitude;
    Msp_sonar_altitude msp_sonar_altitude;
    Msp_attitude msp_attitude;
    Msp_raw_gps msp_raw_gps;
    Msp_comp_gps msp_comp_gps;
    Msp_gps_svinfo msp_gps_svinfo;
    Msp_gps_statistics msp_gps_statistics;
    Msp_nav_status msp_nav_status;
    Msp_nav_poshold msp_nav_poshold;
};

#endif // QUADSTATES_H
