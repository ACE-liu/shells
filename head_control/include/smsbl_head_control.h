/**
 *          @file  head_control.h
 *
 *         @brief  head control module
 *
 *       @version  1.0
 *          @date  1/12/2019 05:15:10 PM
 *        @author  liuliu, liu.liu@iim.ltd
 *
 * @section Description
 *
 *       Revision:  none
 *       Compiler:  g++
 *        Company:  IIM
 *
 * @section Description
 *
 *
 *
 *
 */

#ifndef _SMSBL_HEAD_CONTROL_H_
#define _SMSBL_HEAD_CONTROL_H_


#include "smsbl_motor.h"                                
#include "head_control.h"
#include "type_detection.h"
#include <boost/filesystem.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>


namespace iim_ego {
namespace execution {


const uint8_t SMSBL_ID =1; /**smsbl motor id */

/**
 * @brief Execute the rotation of ego's head
 *
 */
class SmsblHeadControl : public HeadControl
{
public:
    /**
     * @brief constructor
     *
     */
    SmsblHeadControl();
    /**
     * @brief destructor
     *
     */
    ~SmsblHeadControl();

    static bool resetSmsblID(uint8_t  mx28_new_id = SMSBL_ID);
    /**
     * @brief get motor Status
     * @return bool: if true,work well; if false, have fault
     */
    bool getStatus() override;
    static iim_ego::execution::HeadControlType check_if_smsbl_motor();
    
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
     * @brief turn to center point with time delay
     *
     */
    void turn2CenterWithDelay() override;
    /**
     * @brief stop moving
     *
     * @param[out] isStopCenter :  motor status whether it is in center.
     * @return bool
     */
    bool stop_move(bool& isStopCenter) override;

private:
    /**
     * @brief uninitialize dynamixel
     *
     */
    void uninit_smsbl_motor();

    bool init_smsbl_motor();

private:
    smsbl_motor *smsbl_motor_handler; /**pointer to smsbl motor handler*/
    uint8_t motor_id; /**smsbl_motor id for control */
    uint16_t AAC; /**< Acceleration for smsbl motor rotate */
};

}
}

#endif //!_HEAD_CONTROL_H_
