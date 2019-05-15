/**
 *          @file  head_control.h
 *
 *         @brief  head control module
 *
 *       @version  1.0
 *          @date  11/23/2017 04:17:10 PM
 *        @author  Shoey CHI (Robotics), shoey.chi@iim.ltd
 *
 * @section Description
 *
 *       Revision:  none
 *       Compiler:  g++
 *        Company:  IIM
 *
 * @section Description
 *
 * modified by chen jian, shoey , cunjun and liuliu 
 *
 *
 */

#ifndef _DYNAMIXEL_HEAD_CONTROL_H_
#define _DYNAMIXEL_HEAD_CONTROL_H_


#include "dynamixel_sdk.h"                                  // Uses Dynamixel SDK library
#include "head_control.h"
#include "type_detection.h"
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>


namespace iim_ego {
namespace execution {


/**
 * @brief Execute the rotation of ego's head
 *
 */
class DynamixelHeadControl : public HeadControl
{
public:
    /**
     * @brief constructor
     *
     */
    DynamixelHeadControl();
    /**
     * @brief destructor
     *
     */
    ~DynamixelHeadControl();

    static bool resetDynamixelID(uint8_t  mx28_new_id = DXL_ID);
    /**
     * @brief get motor Status
     * @return bool: if true,work well; if false, have fault
     */
    bool getStatus() override;
    
protected:
    /**
     * @brief control motor turn to left
     *
     */
    void turn_to_left() override;
    /**
     * @brief control motor turn to center
     *
     */
    void turn_to_center() override;
    /**
     * @brief control motor turn to right
     *
     */
    void turn_to_right() override;
    /**
     * @brief get motor moving status
     *
     * @return int 0-4
     */
    int  moving_status() override;
    /**
     * @brief control motor turn to center async
     *
     */
    void turn2CenterWithDelay() override;
    /**
     * @brief control motor stop move
     *
     * @param[out] isStopCenter: get if motor is stop center
     * @return bool return false if error occur or return true
     */
    bool stop_move(bool& isStopCenter) override;

private:
    /**
     * @brief uninitialize dynamixel
     *
     */
    void uninit_dynamixel();

    /**
     * @brief initialize dynamixel
     *
     * @return bool: if initialize successfully
     */
    bool init_dynamixel();

private:
    dynamixel::PortHandler *portHandler; /**< port handler*/
    dynamixel::PacketHandler *packetHandler; /**< packet handler */
};
}
}

#endif //
