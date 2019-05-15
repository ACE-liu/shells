// =====================================================================================
// 
//       Filename:  interface_head_control.cpp
// 
//        Version:  1.0
//        Created:  10/13/2017 02:20:31 PM
//       Revision:  none
//       Compiler:  g++
// 
//         Author:  Shoey CHI (Robotics), shoey.chi@iim.ltd
//        Company:  IIM
// 
//    Description:  design interface for head control
// 
// =====================================================================================

#include "interface_head_control.h"
#include <iostream>

static unsigned long long nCnt = 0;

bool
iim_ego::execution::InterfaceHeadControl::onMotor()
{
    std::cout << "execute headcontrol ..." << std::endl;
    return false;
}

void
iim_ego::execution::InterfaceHeadControl::onControl(bool hasFace, perception::detection::RectVectorVectorConstPtr const& side_data)
{
    std::cout<<std::to_string(nCnt) + " round-->head-control ..." << std::endl;
    ++nCnt;
}

bool
iim_ego::execution::InterfaceHeadControl::getStatus()
{
    std::cout << "head-control status ..." <<std::endl;
    return false;
}

bool
iim_ego::execution::InterfaceHeadControl::setHeadControlConf(std::string const& k , std::string const& v)
{
    std::cout<<"set head-control configuration ..." << std::endl;
    return false;
}

std::map<std::string,std::string>
iim_ego::execution::InterfaceHeadControl::getHeadControlConf(std::vector<std::string> const& keys)
{
    std::cout<<"get head-control configuration ..." << std::endl;
    return std::map<std::string,std::string>();
}