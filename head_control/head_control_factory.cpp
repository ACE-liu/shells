#include "head_control_factory.h"
#include <iostream>
#include "empty_head_control.h"
#include "dynamixel_head_control.h"
#include "smsbl_head_control.h"


iim_ego::execution::HeadControl*
iim_ego::execution::headControlFactory::headcontrol = nullptr;

iim_ego::execution::HeadControlType
iim_ego::execution::headControlFactory::motorType = dynamixel;

iim_ego::execution::HeadControl* iim_ego::execution::headControlFactory::createInstance()
{
    HeadControlType checktype =getMotorType();
    if(checktype==smsblmotor)
    {
        std::cout<<"smsbl head motor init success."<<std::endl;
        motorType=smsblmotor;
        headcontrol =new SmsblHeadControl();
    }
    else if(checktype==dynamixel)
    {
        std::cout<<"dynamixel head motor init success."<<std::endl;
        headcontrol =new DynamixelHeadControl();
        motorType=dynamixel;
    }
    else
    {
        std::cout<<"check unknown motor type return EmptyHeadControl."<<std::endl;
        motorType=unknown;
        headcontrol = new EmptyHeadControl();
    }
    return headcontrol;
}

iim_ego::execution::HeadControlType iim_ego::execution::headControlFactory::getMotorType()
{
    return iim_ego::execution::SmsblHeadControl::check_if_smsbl_motor();
}
