#include "quadstates.h"

QuadStates::QuadStates(QByteArray frameid, QByteArray addr_long, QByteArray addr_short)
{
    frame_id = frameid;
    address_long = addr_long;
    address_short = addr_short;

    for (int i=0; i<32; i++)
    {
        active_boxids.box_id[i] = -1;
    }

    msp_ident.MW_VERSION = 0;
    msp_ident.mixer_mode = 0;
    msp_ident.MSP_PROTOCOL_VERSION = 0;
    msp_ident.CAPABILITY = 0;

    msp_loop_time.looptime = 0;

    msp_analog.amp = 0;
    msp_analog.mAhDrawn = 0;
    msp_analog.rssi = 0;
    msp_analog.vbat = 0;

    msp_status.cycletime = 0;
    msp_status.getConfigProfile = 0;
    msp_status.i2cGetErrorCounter = 0;
    msp_status.packFlightModeFlags = 0;
    msp_status.packSensorStatus = 0;

    msp_status_ex.accGetCalibrationAxisFlags = 0;
    msp_status_ex.armingFlags = 0;
    msp_status_ex.averageSystemLoadPercent = 0;
    msp_status_ex.cycletime = 0;
    msp_status_ex.getConfigProfile = 0;
    msp_status_ex.i2cGetErrorCounter = 0;
    msp_status_ex.packFlightModeFlags = 0;
    msp_status_ex.packSensorStatus = 0;

    msp_sensor_flags.acc = false;
    msp_sensor_flags.baro = false;
    msp_sensor_flags.gps = false;
    msp_sensor_flags.hardware = false;
    msp_sensor_flags.mag = false;
    msp_sensor_flags.pitot = false;
    msp_sensor_flags.sonar = false;

    msp_arm_status.ARMED = false;
    msp_arm_status.BLOCK_ACC_NOT_CALIB = false;
    msp_arm_status.BLOCK_COMPASS_NOT_CALIB = false;
    msp_arm_status.BLOCK_HARDWARE_FAILURE = false;
    msp_arm_status.BLOCK_NAV_SAFETY = false;
    msp_arm_status.BLOCK_SENSORS_CALIB = false;
    msp_arm_status.BLOCK_SYSTEM_OVERLOAD = false;
    msp_arm_status.BLOCK_UAV_NOT_LEVEL = false;
    msp_arm_status.OK_TO_ARM = false;
    msp_arm_status.PREVENT_ARMING = false;
    msp_arm_status.UNUSED = false;
    msp_arm_status.WAS_EVER_ARMED = false;

    msp_flight_modes.air_mode = false;
    msp_flight_modes.angle = false;
    msp_flight_modes.arm = false;
    msp_flight_modes.beeper = false;
    msp_flight_modes.blackbox = false;
    msp_flight_modes.camstab = false;
    msp_flight_modes.failsafe = false;
    msp_flight_modes.gcs_nav = false;
    msp_flight_modes.headadj = false;
    msp_flight_modes.headfree = false;
    msp_flight_modes.heading_lock = false;
    msp_flight_modes.home_reset = false;
    msp_flight_modes.horizon = false;
    msp_flight_modes.mag = false;
    msp_flight_modes.nav_althold = false;
    msp_flight_modes.nav_launch = false;
    msp_flight_modes.nav_poshold = false;
    msp_flight_modes.nav_rth = false;
    msp_flight_modes.nav_wp = false;
    msp_flight_modes.osd_sw = false;
    msp_flight_modes.passthru = false;
    msp_flight_modes.surface = false;
    msp_flight_modes.telemetry = false;
    msp_flight_modes.turn_assist = false;

    msp_rc_channels.rcData[0] = 1500;
    msp_rc_channels.rcData[1] = 1500;
    msp_rc_channels.rcData[2] = 1000;
    msp_rc_channels.rcData[3] = 1500;
    msp_rc_channels.rcData[4] = 1000;
    msp_rc_channels.rcData[5] = 1000;
    msp_rc_channels.rcData[6] = 1000;
    msp_rc_channels.rcData[7] = 1000;

    msp_attitude.pitch = 0;
    msp_attitude.roll = 0;
    msp_attitude.yaw = 0;
    /*
    Msp_feature msp_feature;
    Msp_raw_imu msp_raw_imu;
    Temp_mission temp_mission;
    Mission_list mission_list;
    Download_mission_list download_mission_list;
    Msp_altitude msp_altitude;
    Msp_sonar_altitude msp_sonar_altitude;
    Msp_raw_gps msp_raw_gps;
    Msp_comp_gps msp_comp_gps;
    Msp_gps_svinfo msp_gps_svinfo;
    Msp_gps_statistics msp_gps_statistics;
    Msp_nav_status msp_nav_status;
    Msp_nav_poshold msp_nav_poshold;
    */
}
