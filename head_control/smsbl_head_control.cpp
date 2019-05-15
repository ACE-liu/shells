// =====================================================================================
//
//       Filename:  smsbl_head_control.cpp
//
//        Version:  1.0
//        Created:  1/12/2019 05:15:10 PM
//       Revision:  none
//       Compiler:  g++
//
//         Author:  liuliu, liu.liu@iim.ltd
//        Company:  IIM
//
//    Description:  smsbl head control
//
// =====================================================================================

#include "smsbl_head_control.h"
// #include "system_config.h"
// #include "camera_ctrl.h"
// #include "utils.h"
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace
{

int SMSBL_BAUDRATE = 57600;
int SMSBL_SUCCESS = 1;
int smsbl_main_version = 8;
int smsbl_minor_version = 40;
}




iim_ego::execution::SmsblHeadControl::SmsblHeadControl()
{
    this->AAC = mSettings_->AAC;
    bMotorInitSuccess = init_smsbl_motor();
}

iim_ego::execution::SmsblHeadControl::~SmsblHeadControl()
{
    if (bMotorInitSuccess)
        uninit_smsbl_motor();
    if (smsbl_motor_handler) {
        delete smsbl_motor_handler;
        smsbl_motor_handler = nullptr;
    }
}



void iim_ego::execution::SmsblHeadControl::uninit_smsbl_motor()
{
    if (smsbl_motor_handler) {
        smsbl_motor_handler->WritePos(1, center_position_value, mSettings_->nMotorSpeed, AAC);
        sleep(5);
        delete smsbl_motor_handler;
        smsbl_motor_handler = nullptr;
    }
}

int
iim_ego::execution::SmsblHeadControl::moving_status()
{
    uint8_t dxl_error;
    short dxl_comm_result = smsbl_motor_handler->ReadMovingStatus(motor_id);


    if (dxl_comm_result == 0) {
        short dxl_present_position;
        dxl_present_position = smsbl_motor_handler->ReadPos(motor_id, &dxl_error);
        if (dxl_present_position == -1) {
//             logger(IIM_INFO) << "present position: " << dxl_present_position << std::endl;
            return moving;
        } else if (dxl_error != 0) {
//             logger(IIM_INFO) << "[hdw] present position: " << dxl_present_position << std::endl;
            return moving;
        }
        if (std::abs(left_position_value - dxl_present_position) < mSettings_->nMovingStatusThreshold)
            return stop_left;
        else if (std::abs(right_position_value - dxl_present_position) < mSettings_->nMovingStatusThreshold)
            return stop_right;
        else if (std::abs(center_position_value - dxl_present_position) < mSettings_->nMovingStatusThreshold)
            return stop_center;
        else
            return stop_somewhere;
    } 
    return moving;
}


void iim_ego::execution::SmsblHeadControl::turn_to_left()
{
    int error = 0;
    int dxl_comm_result = smsbl_motor_handler->WritePos(motor_id, left_position_value, mSettings_->nMotorSpeed, AAC, &error);
    if (dxl_comm_result != SMSBL_SUCCESS)
       std::cout<< "fail to comm WRITE turn left: " << smsbl_motor_handler->getErrorStringbytype(error) << std::endl;
//     logger(IIM_INFO) << "turn_to_left." << std::endl;
}

void iim_ego::execution::SmsblHeadControl::turn_to_center()
{
    int error = 0;
    int dxl_comm_result = smsbl_motor_handler->WritePos(motor_id, center_position_value, mSettings_->nMotorSpeed, AAC, &error);
    if (dxl_comm_result != SMSBL_SUCCESS)
       std::cout<< "fail to comm WRITE turn center: " << smsbl_motor_handler->getErrorStringbytype(error) << std::endl;
//     logger(IIM_INFO) << "turn_to_center." << std::endl;
}

void
iim_ego::execution::SmsblHeadControl::turn2CenterWithDelay()
{
    boost::async(boost::launch::async, [&] {
        boost::this_thread::sleep_for(boost::chrono::milliseconds(mSettings_->nDelayTime));
        //uint8_t dxl_error;
        int error = 0;
        int dxl_comm_result = smsbl_motor_handler->WritePos(motor_id, center_position_value, mSettings_->nMotorSpeed, AAC, &error);
        if (dxl_comm_result != SMSBL_SUCCESS)
            std::cout << "fail to comm: turn2CenterWithDelay:" << smsbl_motor_handler->getErrorStringbytype(error) << std::endl;
        nMoveStatus = moving;
        while (nMoveStatus == moving)
        {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(mSettings_->nDelayTime));
            nMoveStatus = moving_status();
        }
        boost::this_thread::sleep_for(boost::chrono::seconds(3));
        bReturnCenter = false;
    });
    std::cout<< "turn_to_center_with_delay." << std::endl;
}


void iim_ego::execution::SmsblHeadControl::turn_to_right()
{
    int error = 0;
    int dxl_comm_result = smsbl_motor_handler->WritePos(motor_id, right_position_value, mSettings_->nMotorSpeed, AAC, &error);
    if (dxl_comm_result != SMSBL_SUCCESS)
        std::cout << "fail to comm WRITE turn right: " << smsbl_motor_handler->getErrorStringbytype(error) << std::endl;

//     logger(IIM_INFO) << "turn_to_right." << std::endl;
}

bool iim_ego::execution::SmsblHeadControl::resetSmsblID(uint8_t mx28_new_id)
{
    smsbl_motor smsbl_motor;
    if (!smsbl_motor.initialize(SMSBL_BAUDRATE, "/dev/motor0")) {
        std::cout << "Failed to init smsbl motor!" << std::endl;
        return false;
    }
    int result = smsbl_motor.Ping(mx28_new_id);
    if (result == mx28_new_id) {
        std::cout << "the current ID is same as the new ID!" << std::endl;
        return true;
    } else if (result == -1) {
        std::cout << "timeout !" << std::endl;
        return false;
    }
    return true;
}

iim_ego::execution::HeadControlType iim_ego::execution::SmsblHeadControl::check_if_smsbl_motor()
{
    smsbl_motor smsbl_motor;
    if (!smsbl_motor.initialize(SMSBL_BAUDRATE, "/dev/motor0")) {
        std::cout << "Failed to init smsbl motor!" << std::endl;
        return unknown;
    }
    int motorid = 1;
    int main_ver, minor_ver;
    main_ver = smsbl_motor.ReadMainVersion(motorid);
    minor_ver = smsbl_motor.ReadMinorVersion(motorid);
    int count = 0;
    while ((main_ver == -1 || minor_ver == -1) && count < 5) {
        main_ver = smsbl_motor.ReadMainVersion(motorid);
        minor_ver = smsbl_motor.ReadMinorVersion(motorid);
        count++;
        std::cout << "sms read version failed ....................." << std::endl;
    }
    if (count == 5) {
        std::cout << "sms_motor read version error 5 times..." << std::endl;
        return unknown;
    }
    if (main_ver != smsbl_main_version || minor_ver != smsbl_minor_version)
        return dynamixel;
    else
        return smsblmotor;
}

bool iim_ego::execution::SmsblHeadControl::init_smsbl_motor()
{
    smsbl_motor_handler = new smsbl_motor();
    if (!smsbl_motor_handler->initialize(SMSBL_BAUDRATE, "/dev/motor0")) {
//         logger(IIM_INFO) << "serial port is not ready. Failed to init smsbl motor!" << std::endl;
        return false;
    }
    motor_id = 1;
    // uint8_t new_motor_id= smsbl_motor_handler->Ping(motor_id);
    turn_to_center();
//     logger(IIM_INFO) << "smsbl init done, direction_priority = " << mSettings_->motor_turn_priority << std::endl;
    return true;
}

bool
iim_ego::execution::SmsblHeadControl::stop_move(bool& isStopCenter)
{
    uint8_t dxl_error;
    short curr_position = 0x1;
    int rtn = 0;
    short speed;
    bool hasStop = false;
    speed = smsbl_motor_handler->ReadSpeed(motor_id, &dxl_error);
    if (dxl_error == 0 && speed == 0)
        hasStop = true;
    if (!hasStop) {
        rtn = smsbl_motor_handler->WriteSpe(motor_id, 0);
        if (rtn != SMSBL_SUCCESS)
            return false;
    }
    curr_position = smsbl_motor_handler->ReadPos(motor_id, &dxl_error);
    if (dxl_error != 0) {
//         logger(IIM_INFO) << "fail to read position " << std::endl;
        return false;
    }
//     logger(IIM_INFO) << "success to stop!" << std::endl;
    if (std::abs(center_position_value - curr_position) < mSettings_->nMovingStatusThreshold)
        isStopCenter = true;
    else
        isStopCenter = false;
    return true;
}


bool
iim_ego::execution::SmsblHeadControl::getStatus()
{
    //uint16_t dxl2_moving_status = 0;                        // Dynamixel moving status
    uint8_t dxl_error;
    int dxl_comm_result = smsbl_motor_handler->ReadPos(motor_id, &dxl_error);
    if (dxl_comm_result == -1 || dxl_error == 1) {
        dxl_comm_result = smsbl_motor_handler->ReadPos(motor_id, &dxl_error);
    }
    return (dxl_comm_result == SMSBL_SUCCESS) && (dxl_error == 0) ;
}

