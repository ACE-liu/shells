#include "dynamixel_head_control.h"
// #include "system_config.h"
// #include "camera_ctrl.h"
// #include "utils.h"
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>



// Control table address
namespace {
const float PROTOCOL_VERSION = 1.0f;                 // See which protocol version is used in the Dynamixel
uint16_t ADDR_MX_TORQUE_ENABLE = 24;                  // Control table address is different in Dynamixel model
uint16_t ADDR_MX_GOAL_POSITION = 30;
uint16_t DXL_GOAL_SPEED_LOW = 32;
uint16_t ADDR_MX_PRESENT_POSITION = 36;
uint16_t ADDR_MX_MOVING = 46;
int  BAUDRATE = 57600;
uint8_t TORQUE_ENABLE = 1;                   // Value for enabling the torque
uint8_t TORQUE_DISABLE = 0;                   // Value for disabling the torque

}

/**
 * @brief iim_ego::execution::HeadControl::HeadControl
 */


iim_ego::execution::DynamixelHeadControl::DynamixelHeadControl()
{
    bMotorInitSuccess = init_dynamixel();
}

iim_ego::execution::DynamixelHeadControl::~DynamixelHeadControl()
{
    if(bMotorInitSuccess)
        uninit_dynamixel();
}



void iim_ego::execution::DynamixelHeadControl::uninit_dynamixel()
{
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE,
                                                        TORQUE_DISABLE, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS) {
        std::cout<<"In HeadControl, Disable Dynamixel Torque: " << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
    } else if (dxl_error != 0) {
        std::cout<<"In HeadControl, Disable Dynamixel Torque: " << packetHandler->getRxPacketError(dxl_error)<<std::endl;
    }
    portHandler->closePort();
}

int
iim_ego::execution::DynamixelHeadControl::moving_status()
{
    uint16_t dxl2_moving_status = 0;                        // Dynamixel moving status
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_MOVING, &dxl2_moving_status,
                                                       &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
//         logger(IIM_INFO) <<"moving_status: "<< packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
        return moving;
    }
    else if (dxl_error != 0)
    {
//         logger(IIM_INFO) <<"[hdw] moving_status: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
        return moving;
    }

    if (dxl2_moving_status == 0)
    {
        uint16_t dxl_present_position;
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_PRESENT_POSITION,
                                                       &dxl_present_position, &dxl_error);
        if (dxl_comm_result != COMM_SUCCESS)
        {
//             logger(IIM_INFO)<< "present position: "<<packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
            return moving;
        }
        else if (dxl_error != 0)
        {
//             logger(IIM_INFO) << "[hdw] present position: "<<packetHandler->getRxPacketError(dxl_error) << std::endl;
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
    } else
        std::cout << "under moving ..." << std::endl;
    return moving;
}


void iim_ego::execution::DynamixelHeadControl::turn_to_left()
{
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION,
                                                        left_position_value,&dxl_error);
    if(dxl_error != 0)
        std::cout <<"[hdw] fail to comm: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
    else if(dxl_comm_result != COMM_SUCCESS)
        std::cout<< "fail to comm:" << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;

//     logger(IIM_INFO) << "turn_to_left." << std::endl;
}

void iim_ego::execution::DynamixelHeadControl::turn_to_center()
{  
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION,
                                                        center_position_value,&dxl_error);
    if(dxl_error != 0)
        std::cout <<"[hdw] fail to comm: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
    else if(dxl_comm_result != COMM_SUCCESS)
        std::cout << "fail to comm:" << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
//     logger(IIM_INFO)<< "turn_to_center."<<std::endl;
}

void
iim_ego::execution::DynamixelHeadControl::turn2CenterWithDelay()
{
    boost::async(boost::launch::async, [&]{
        boost::this_thread::sleep_for(boost::chrono::milliseconds(mSettings_->nDelayTime));
        uint8_t dxl_error;
        int dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION,
                                                            center_position_value,&dxl_error);
        if(dxl_error != 0)
            std::cout <<"[hdw] fail to comm: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
        else if(dxl_comm_result != COMM_SUCCESS)
            std::cout << "fail to comm:" << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
        nMoveStatus = moving;
        while(nMoveStatus == moving) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds(mSettings_->nDelayTime));
            nMoveStatus = moving_status();
        }
        boost::this_thread::sleep_for(boost::chrono::seconds(3));
        bReturnCenter = false;
    });
   std::cout<< "turn_to_center_with_delay."<<std::endl;
}


void iim_ego::execution::DynamixelHeadControl::turn_to_right()
{
    uint8_t dxl_error;
    int  dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, ADDR_MX_GOAL_POSITION,
                                                         right_position_value,&dxl_error);
    if(dxl_error != 0)
       std::cout<<"[hdw] fail to comm: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
    else if(dxl_comm_result != COMM_SUCCESS)
       std::cout<< "fail to comm:" << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
    std::cout << "turn_to_right."<<std::endl;
}

bool iim_ego::execution::DynamixelHeadControl::resetDynamixelID(uint8_t mx28_new_id)
{
    // Initialize PortHandler instance, Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    dynamixel::PortHandler* portHandler2 = dynamixel::PortHandler::getPortHandler("/dev/motor0" );
    // Initialize PacketHandler instance, Set the protocol version
    // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
    dynamixel::PacketHandler* packetHandler2 = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    if (portHandler2->openPort()) {
        std::cout << "In HeadControl, Succeeded to open the port!\n";
    } else {
        std::cout << "In HeadControl, Failed to open the port! check the motor device: /dev/motor0\n";
        return false;
    }
    if (portHandler2->setBaudRate(BAUDRATE)) {
        std::cout << "In HeadControl, Succeeded to change the baudrate!\n";
    } else {
        std::cout <<"In HeadControl, Failed to change the baudrate!\n";
        return false;
    }

    uint8_t error2 = 0;
    if(packetHandler2->ping(portHandler2,mx28_new_id,&error2) == COMM_SUCCESS)
    {
        std::cout << "the current ID is same as the new ID!"<<std::endl;
        return true;
    }

    uint8_t id = 0;
    for(; id < 254; ++id)
    {
        if(packetHandler2->ping(portHandler2,id,&error2) == COMM_SUCCESS)
            break;
    }
    if(id >= 254)
    {
        std::cout << "no such a port!" << std::endl;
        return false;
    }
    //for firmware version 1.0
    int dxl_comm_result2  = packetHandler2->writeTxOnly(portHandler2,id,3,1,&mx28_new_id);
    if (dxl_comm_result2 != COMM_SUCCESS)
        std::cout<<"send data failed!"<<std::endl;
    else
        std::cout<<"send data successfully!"<<std::endl;
    return true;
}

bool iim_ego::execution::DynamixelHeadControl::init_dynamixel()
{
    // Initialize PortHandler instance, Set the port path
    // Get methods and members of PortHandlerLinux or PortHandlerWindows
    portHandler = dynamixel::PortHandler::getPortHandler("/dev/motor0");
    // Initialize PacketHandler instance, Set the protocol version
    // Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
    packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);
    if (portHandler->openPort()) {
        std::cout << "Succeeded to open the port!"<<std::endl;
    } else {
        std::cout<< "Failed to open the port! check the motor device: /dev/motor0"<<std::endl;
        return false;
    }
    if (portHandler->setBaudRate(BAUDRATE)) {
        std::cout << "Succeeded to change the baudrate!"<<std::endl;
    } else {
        std::cout << "Failed to change the baudrate!"<<std::endl;
        return false;
    }
    // Enable Dynamixel Torque
    uint8_t dxl_error;
    int dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_MX_TORQUE_ENABLE,
                                                        TORQUE_ENABLE, &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
//         logger(IIM_INFO) << "Enable Dynamixel Torque failed: "<< packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
        return false;
    }
    else if (dxl_error != 0)
    {
//         logger(IIM_INFO) << "[hdw] Enable Dynamixel Torque failed: "<<packetHandler->getRxPacketError(dxl_error) << std::endl;
        return false;
    }


    turn_to_center();

    // set_motor_speed
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, DXL_GOAL_SPEED_LOW, mSettings_->nMotorSpeed, &dxl_error);
//     logger(IIM_INFO) << "Dynamixel init done, direction_priority = " << mSettings_->motor_turn_priority << std::endl;
    return true;
}



bool
iim_ego::execution::DynamixelHeadControl::stop_move(bool& isStopCenter)
{
    uint8_t dxl_error;
    uint16_t curr_position = 0x1;
    int dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID, DXL_GOAL_SPEED_LOW, curr_position,&dxl_error);
    dxl_comm_result = packetHandler->read2ByteTxRx(portHandler,DXL_ID, ADDR_MX_PRESENT_POSITION, &curr_position, &dxl_error);
    if(dxl_error != 0)
    {
//         logger(IIM_INFO)<<"[hdw] fail to stop: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
        return false;
    }
    else if(dxl_comm_result != COMM_SUCCESS)
    {
//         logger(IIM_INFO)<<"fail to stop: "<<packetHandler->getRxPacketError(dxl_error)<<std::endl;
        return false;
    }
    
    dxl_comm_result = packetHandler->write2ByteTxRx(portHandler, DXL_ID,ADDR_MX_GOAL_POSITION, curr_position,&dxl_error);

    if(dxl_error != 0)
    {
//         logger(IIM_INFO) <<"[hdw] fail to stop: "<< packetHandler->getRxPacketError(dxl_error) << std::endl;
        return false;
    }
    else if(dxl_comm_result != COMM_SUCCESS)
    {
//         logger(IIM_INFO) << "fail to stop:" <<packetHandler->getTxRxResult(dxl_comm_result) << std::endl;
        return false;
    }


    boost::this_thread::sleep_for(boost::chrono::milliseconds(mSettings_->nDelayTime));
    uint32_t union_val = mSettings_->nMotorSpeed;
    union_val <<=16;
    union_val |= curr_position;
    dxl_comm_result = packetHandler->write4ByteTxRx(portHandler, DXL_ID,ADDR_MX_GOAL_POSITION, union_val,&dxl_error);

    if(dxl_error != 0)
    {
//         logger(IIM_INFO) <<"[hdw] fail to stop: "<< packetHandler->getRxPacketError(dxl_error)<<std::endl;
        return false;
    }
    else
    {
        if(dxl_comm_result == COMM_SUCCESS)
        {
//             logger(IIM_INFO)<< "success to stop!" << std::endl;
            if(std::abs(center_position_value - curr_position) < mSettings_->nMovingStatusThreshold)
                isStopCenter = true;
            else
                isStopCenter = false;
            return true;
        }
        else
        {
//             logger(IIM_INFO)<< "fail to stop:" << packetHandler->getTxRxResult(dxl_comm_result)<<std::endl;
            return false;
        }
    }
}


bool
iim_ego::execution::DynamixelHeadControl::getStatus()
{
    uint16_t dxl2_moving_status = 0;                        // Dynamixel moving status
    uint8_t dxl_error;

    int dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_MOVING, &dxl2_moving_status,
                                                       &dxl_error);
    if (dxl_comm_result != COMM_SUCCESS)
    {
        dxl_comm_result = packetHandler->read2ByteTxRx(portHandler, DXL_ID, ADDR_MX_MOVING, &dxl2_moving_status,
                                                       &dxl_error);
    }
    return dxl_comm_result == COMM_SUCCESS;
}

