/**
 *          @file  head_control_factory.h
 *
 *         @brief  head-control factory to create an instance
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

#ifndef _HEAD_CONTROL_FACTORY_H_
#define _HEAD_CONTROL_FACTORY_H_

#include "head_control.h"

namespace iim_ego {
namespace execution {


/**
 * @brief class for head control factory
 *
 */
class headControlFactory
{
public:
    /**
     * @brief create an instance
     *
     * @return HeadControl
     */
    static HeadControl* createInstance();
    static HeadControlType motorType; /** save current using motor type */
private:
    static HeadControl *headcontrol; /** pointer to current headcontrol */
    /**
     * @brief get motor type
     *
     * @return HeadControlType
     */
    static HeadControlType getMotorType();
};


}
}





#endif
