/**
 *          @file  head_control_settings.h
 *
 *         @brief  head control settings
 *
 *       @version  1.0
 *          @date  11/dec/2018 17:35:18
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
 * -->describe more here<--
 *
 */

#ifndef _HEAD_CONTROL_SETTINGS_H_
#define _HEAD_CONTROL_SETTINGS_H_

// #include "logstream.h"
#include <string>

namespace iim_ego {
namespace execution {
	
/**
 * @brief set parameters of head control
 *
 */
class HeadControlSettings
{
public:
    /**
     * @brief destructor
     *
     */
    ~HeadControlSettings() = default;
    /**
     * @brief constructor
     *
     */
    HeadControlSettings();
    /**
     * @brief constructor
     *
     * @param[in] filename: file storing HeadControlSettings
     */
    HeadControlSettings(std::string const& filename);
    void saveSettings(std::string const& filename);
    std::string motor_turn_priority; /**< preferred direction to turn when both side cameras capture data at first time*/
    int motor_angle_start; /**< start angle of motor */
    int motor_angle_center; /**< center angle of motor */
    int motor_angle_end; /**< end angle of motor */
    bool onAnalysis_cam_right; /**< if right camera is on */
    bool onAnalysis_cam_left; /**< if left camera is on */
    uint16_t nMotorSpeed; /**< rotation speed of motor */
    bool bMotorTurnOver; /**< the state  of motor turn over*/
    int nDelayTime; /**< turn-to-center delay time*/
    bool bMotorStopWithFace; /**< whether turn to center when main camera detected faces*/
    uint16_t nMovingStatusThreshold;  /**< Dynamixel moving status threshold*/
    uint16_t  AAC;  

    /**
     * @brief define assignment operation for HeadControlSettings
     *
     * @param[in] obj: object containing HeadControlSettings
     * @return HeadControlSettings &operator
     */
    HeadControlSettings& operator = (HeadControlSettings const& obj);
    /**
     * @brief parse the status of two side cameras
     *
     * @param[in] iToggle: side cameras status
     */
    void parse(int iToggle);
    
	
private:
     /**
     * @brief load motor settings stored in the file
     *
     * @param[in] filename: file storing motor settings
     */
     void loadSettings(std::string const& filename);
     
    /**
     * @brief define operator for data output
     *
     * @param[in] os: ostream
     * @param[in] data: data containing HeadControlSettings
     * @return std::ostream &operator
     */
    friend std::ostream& operator<<(std::ostream& os, HeadControlSettings const& data);
};

/**
 * @brief  define operator for data output
 *
 * @param[in/out] os: ostream
 * @param[in] data: data containing HeadControlSettings
 * @return std::ostream &operator
 */
std::ostream& operator<<(std::ostream& os, HeadControlSettings const& data);

	
}
}
#endif //!_HEAD_CONTROL_SETTINGS_H_


