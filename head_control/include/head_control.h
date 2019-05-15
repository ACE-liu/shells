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
 * modified by chen jian, shoey and cunjun
 *
 *
 */

#ifndef _HEAD_CONTROL_H_
#define _HEAD_CONTROL_H_

#include "interface_head_control.h"
#include "type_detection.h"
#include "head_control_settings.h"
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
// #include "logstream.h"
#include <string>


namespace iim_ego {
namespace execution {


const uint8_t DXL_ID = 1;                    /**Dynamixel motor id */
enum HeadControlType
{
    unknown=0,
    dynamixel,
    smsblmotor,
};


/**
 * @brief Execute the rotation of ego's head
 *
 */
class HeadControl : public InterfaceHeadControl
{
protected:
    enum
    {
        moving = 0,
        stop_left = 1,
        stop_center = 2,
        stop_right = 3,
        stop_somewhere = 4
    };

public:
    /**
     * @brief destructor
     *
     */
    ~HeadControl();
    /**
     * @brief  send command to Controller
     *
     * @param[in] hasFace: if face is detected by main camera
     * @param[in] side_data: body detection data obtained from sideview cameras
     */
    void onControl(bool hasFace, perception::detection::RectVectorVectorConstPtr const& side_data) override;
    /**
     * @brief Head rotation by dynamixel motor
     * @return bool : if true, execute one more time then wait to notify
     *
     */
    bool onMotor() override;
    /**
     * @brief parse the data obtained from side camera
     *
     * @param[in] data: body detection data obtained from sideview cameras
     */
    void parse_sideviewInfo(perception::detection::RectVectorVectorConstPtr const&data);
    /**
     * @brief set parameters for HeadControlSettings
     *
     * @param[in] obj: instance of HeadControlSettings
     */
    static void setParam(HeadControlSettings const& obj);
    /**
     * @brief return HeadControlSettings
     *
     * @return HeadControlSettings
     */
    static HeadControlSettings& getParam();
    /**
     * @brief reset dynamixel mx-28 ID. It works for the  firmware version 1.0
     * @param[in] mx28_new_id: new ID
     * @return bool: if reset is successful
     */
    static bool resetDynamixelID(uint8_t  mx28_new_id = DXL_ID);
    /**
     * @brief interface function for setting configurations of motor
     * 
     * @return bool
     */
    bool setHeadControlConf(std::string const& k, std::string const& v) override;
    /**
     * @brief interface function for getting configurations of motor
     *
     * @return std::map<std::string,std::string>
     */
     std::map<std::string,std::string> getHeadControlConf(std::vector<std::string> const& keys) override;
protected:
    /**
     * @brief constructor
     *
     */
    HeadControl();
    /**
     * @brief interface function for control motor turn to left
     *
     */
    virtual void turn_to_left()=0;
    /**
     * @brief interface function for control motor turn to center
     *
     */
    virtual void turn_to_center()=0;
    /**
     * @brief interface function for control motor turn to right
     *
     */
    virtual void turn_to_right()=0;
    /**
     * @brief interface function for get motor moving status
     *
     * @return int
     */
    virtual int  moving_status()=0;
    /**
     * @brief interface function for control motor turn to center async
     *
     */
    virtual void turn2CenterWithDelay()=0;
    /**
     * @brief interface function for control motor stop move
     *
     * @param[out] isStopCenter: get if motor is stop center
     * @return bool return false if error occur or return true
     */
    virtual bool stop_move(bool& isStopCenter)=0;
private:
    /**
     * @brief convert setting angles to motor readable values
     *
     */
    static void transform_angle_to_value(std::ostream& os);

    /**
     * @brief turn head to center when it stops somewhere while there has no face
     *
     */
    void turn_back_center_without_face();
    
    
protected:
    /**
     * @brief configure motor settings
     *
     */
    void config_motor_with_settings();
    static uint16_t center_position_value; /**< center position value of motor */
    static uint16_t right_position_value; /**< right position value of motor */
    static uint16_t left_position_value; /**< left position value of motor */
    static HeadControlSettings* mSettings_; /**< pointer to HeadControlSettings */
    static double factor; /**< coefficient to convert angle to position value*/
    static uint16_t center_value; /**< center value of motor */
    bool bReturnCenter; /**< if the head can return back to center */
    bool bMotorInitSuccess;  /**< if the motor has been inited successfully*/
    int nMoveStatus;  /**< robot head status */
//     basis::logstream logger; /**< log file  */

private:
    boost::chrono::seconds const TimeoutLimit; /**< timelimit for head to stay in one direction while no face is detected */
    bool hasFace; /**< if face is detected by main camera */
    bool bTurnable; /**< if the executor can accept next command */
    bool nTurnDirection, previous_turn_direction; /**< whether turn to left or right,turn to left or right last time  */
    boost::thread *thd_turn_back_center;   /**< a thread to turn head back center when it stops somewhere while no data detected*/
    boost::mutex mtx_;
    boost::condition_variable cv_turn_back;
};
}
}

#endif //!_HEAD_CONTROL_H_
