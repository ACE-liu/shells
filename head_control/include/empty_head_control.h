/**
 *          @file  empty_head_control.h
 *
 *         @brief  empty inheritance of head-control
 *
 *       @version  1.0
 *          @date  17/Jan/2019 00:16:59
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

#ifndef _EMPTY_HEAD_CONTROL_H_
#define _EMPTY_HEAD_CONTROL_H_

#include "head_control.h"

namespace iim_ego {
namespace execution {

class EmptyHeadControl : public HeadControl
{
public:
    EmptyHeadControl() {}
    ~EmptyHeadControl() = default;
    /**
     * @brief get default status
     * @return bool:
     */
    bool getStatus() override
    {
        return true;
    }
protected:
    void turn_to_left() override
    {}
    /**
     * @brief default function for control motor turn to center
     *
     */
    void turn_to_center() override
    {}
    /**
     * @brief default function for control motor turn to right
     *
     */
    void turn_to_right() override
    {}
    /**
     * @brief default function for get motor moving status
     *
     * @return int
     */
    int  moving_status() override
    {
        return stop_center;
    }
    /**
     * @brief default function for control motor turn to center async
     *
     */
    void turn2CenterWithDelay() override
    {
    }
    /**
     * @brief default function for control motor stop move
     *
     * @param[out] isStopCenter: get if motor is stop center
     * @return bool return false if error occur or return true
     */
    bool stop_move(bool& isStopCenter) override
    {
        return false;
    }
};

}
}
#endif //!_EMPTY_HEAD_CONTROL_H_


