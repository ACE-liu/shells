// =====================================================================================
// 
//       Filename:  head_control_settings.cpp
// 
//        Version:  1.0
//        Created:  11/dec/2018 17:36:03
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Shoey CHI (Robotics), shoey.chi@iim.ltd
//        Company:  IIM
// 
//    Description:  head control settings
// 
// =====================================================================================

#include "head_control_settings.h"
#include <iostream>
#include <boost/filesystem.hpp>
// #include <glog/logging.h>
#include <opencv2/core/persistence.hpp>
#include <iosfwd>

std::ostream& iim_ego::execution::operator << (std::ostream& os, iim_ego::execution::HeadControlSettings const& data)
{
    os <<"motor_turn_priority: " << data.motor_turn_priority << std::endl;
    os <<"onAnalysis_cam_left: " << data.onAnalysis_cam_left << std::endl;
    os <<" onAnalysis_cam_right: " << data.onAnalysis_cam_right << std::endl;
    os <<" motor_angle_center: " <<  data.motor_angle_center << std::endl;
    os <<"motor_angle_start: " << data.motor_angle_start << std::endl;
    os <<"motor_angle_end: " << data.motor_angle_end << std::endl;
    os << "MotorSpeed: " << data.nMotorSpeed << std::endl;
    os << "AAC: " << data.AAC << std::endl;
    os << "motor_stop_with_face: " << (data.bMotorStopWithFace ? "true" : "false") << std::endl;
    return os;
}

iim_ego::execution::HeadControlSettings::HeadControlSettings()
{
    motor_turn_priority = "right";
    motor_angle_center = 0;
    motor_angle_start = 60;
    motor_angle_end = -60;
    onAnalysis_cam_left = true;
    onAnalysis_cam_right = true;
    bMotorStopWithFace = false;
    //once setting
    nMotorSpeed = 60;
    AAC=5;
    nDelayTime  =150;//ms
    nMovingStatusThreshold = 80;
    //no setting variable
    bMotorTurnOver = false;
}

iim_ego::execution::HeadControlSettings&
iim_ego::execution::HeadControlSettings::operator = (HeadControlSettings const& obj)
{
    this->motor_turn_priority = obj.motor_turn_priority;
    this->onAnalysis_cam_left = obj.onAnalysis_cam_left;
    this->onAnalysis_cam_right = obj.onAnalysis_cam_right;
    this->motor_angle_center = obj.motor_angle_center;
    this->motor_angle_end = obj.motor_angle_end;
    this->motor_angle_start = obj.motor_angle_start;
    this->bMotorStopWithFace = obj.bMotorStopWithFace;
    return *this;
}

iim_ego::execution::HeadControlSettings::HeadControlSettings(std::string const& filename)    
{
    if(!boost::filesystem::exists(filename))
    {
        std::cout<<filename << " not exits..."<<std::endl;
    }
    loadSettings(filename + ".usr");
    parse(1);
    bMotorTurnOver = false;
}

void
iim_ego::execution::HeadControlSettings::parse(int iToggle)
{

        onAnalysis_cam_left = true;
        onAnalysis_cam_right = true;
}

void iim_ego::execution::HeadControlSettings::loadSettings(std::string const& filename)
{
    cv::FileStorage fs(filename, cv::FileStorage::FORMAT_XML);
    if (!fs.isOpened()) {
        std::cout << "Cannot open file: " << filename << std::endl;
        return;
    }
    cv::FileNode node;
    node = fs["motor_angle_center"];
    if (node.isNone())
        motor_angle_center = 0;
    else
        node >> motor_angle_center;

    node = fs["motor_angle_start"];
    if (node.isNone())
        motor_angle_end = 45;
    else
        node >> motor_angle_start;

    node = fs["motor_angle_end"];
    if (node.isNone())
        motor_angle_end = -45;
    else
        node >> motor_angle_end;

    node = fs["motor_turn_priority"];
    if (node.isNone())
        motor_turn_priority = "right";
    else
        node >> motor_turn_priority;

    node = fs["motor_speed"];
    if (node.isNone())
        nMotorSpeed = 80;
    else
        node >> nMotorSpeed;

    node = fs["AAC"];
    if (node.isNone())
        AAC =5;
    else
        node >> AAC;

    node = fs["delay_time"];
    if (node.isNone())
        nDelayTime = 150;
    else
        node >> nDelayTime;

    node = fs["moving_status"];
    if (node.isNone())
        nMovingStatusThreshold = 80;
    else
        node >> nMovingStatusThreshold;

    node = fs["motor_stop_with_face"];
    if (node.isNone())
        bMotorStopWithFace = false;
    else
        node >> bMotorStopWithFace;
    fs.release();
}

void iim_ego::execution::HeadControlSettings::saveSettings(std::string const& filename)
{
    cv::FileStorage fs(filename + ".usr", cv::FileStorage::FORMAT_XML | cv::FileStorage::WRITE);
    if (!fs.isOpened())
    {
        std::cout << "Cannot open file: " << filename << std::endl;
        return;
    }
    fs << "motor_angle_center" << motor_angle_center;
    fs << "motor_angle_start" << motor_angle_start;
    fs << "motor_angle_end" << motor_angle_end;
    fs << "motor_turn_priority" << motor_turn_priority;
    fs << "motor_speed" << nMotorSpeed;
    fs << "AAC" << AAC;
    fs << "delay_time" << nDelayTime;
    fs << "moving_status" << nMovingStatusThreshold;
    fs << "motor_stop_with_face" << (int)bMotorStopWithFace;
    fs.release();
}
