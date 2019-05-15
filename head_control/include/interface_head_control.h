/**
 *          @file  interface_head_control.h
 *
 *         @brief  design interface for head control
 *
 *       @version  1.0
 *          @date  10/13/2017 02:18:27 PM
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

#ifndef _INTERFACE_HEAD_CONTROL_H_
#define _INTERFACE_HEAD_CONTROL_H_
#include <boost/shared_ptr.hpp>
#include "type_detection.h"
#include <string>
#include <vector>
#include <map>

namespace iim_ego{
namespace execution{

/**
 * @brief interface of head-control
 *
 */
class InterfaceHeadControl
{
public:
    /**
     * @brief shared pointer of this class
     *
     */
    typedef boost::shared_ptr<InterfaceHeadControl> Ptr;
    /**
     * @brief destructor
     *
     */
    virtual ~InterfaceHeadControl() = default;
    /**
     * @brief interface function for transitting & pre-processing data
     *
     * @param[in] hasFace : flag whether to have face
     * @param[in] side_data : vector of body detected results
     */
    virtual void onControl(bool hasFace,perception::detection::RectVectorVectorConstPtr const& side_data) = 0;
    /**
     * @brief interface function for motor execution
     *
     * @return bool
     */
    virtual bool onMotor() = 0;
    /**
     * @brief interface function for getting status of motor
     *
     * @return bool
     */
    virtual bool getStatus() = 0;
    /**
     * @brief interface function for setting configurations of motor
     * 
     * @return bool
     */
     virtual bool setHeadControlConf(std::string const& k, std::string const& v) = 0;
    /**
     * @brief interface function for getting configurations of motor
     *
     * @return std::map<std::string,std::string>
     */
     virtual std::map<std::string,std::string> getHeadControlConf(std::vector<std::string> const& keys) = 0;
protected:
    /**
     * @brief constructor
     *
     */
    InterfaceHeadControl() = default;
};

}
}
#endif //!_INTERFACE_HEAD_CONTROL_H_
