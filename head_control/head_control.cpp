#include "head_control.h"
// #include "system_config.h"
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
// #include "utils.h"



/**
 * @brief iim_ego::execution::HeadControl::HeadControl
 */

iim_ego::execution::HeadControlSettings*
iim_ego::execution::HeadControl::mSettings_ = new iim_ego::execution::HeadControlSettings();

void
iim_ego::execution::HeadControl::setParam(HeadControlSettings const& obj)
{
    *mSettings_ = obj;
//     std::string file = core::SystemConfig::getConstPtrInstance()->getRuntimePath() + "/execution/head_control/head_control.xml";
//     mSettings_->saveSettings(file);
    transform_angle_to_value(std::cout);
}

iim_ego::execution::HeadControlSettings&
iim_ego::execution::HeadControl::getParam()
{
    return *mSettings_;
}

bool iim_ego::execution::HeadControl::setHeadControlConf(std::string const& k, std::string const& v)
{
    if (k == "ROBOT_START_TURNING_ANGLE") {
        int iv = std::stoi(v);
        if (iv > 60 || iv < -60)
            return false;
        mSettings_->motor_angle_start = iv;
    } else if (k == "ROBOT_END_TURNING_ANGLE") {
        int iv = std::stoi(v);
        if (iv > 60 || iv < -60)
            return false;
        mSettings_->motor_angle_end = iv;
    } else if (k == "ROBOT_RIGHT_CAM_ENABLE") {
        if(v == "true")
            mSettings_->onAnalysis_cam_right = true;
        else
            mSettings_->onAnalysis_cam_right = false;
    } else if (k == "ROBOT_LEFT_CAM_ENABLE") {
        if(v == "true")
            mSettings_->onAnalysis_cam_left = true;
        else
            mSettings_->onAnalysis_cam_left = false;
    } else
        return false;
    setParam(*mSettings_);
    return true;
}

std::map<std::string,std::string> iim_ego::execution::HeadControl::getHeadControlConf(std::vector<std::string> const& keys)
{
    std::map<std::string,std::string> key_values;
    for(std::string const& k : keys)
    {
        if(k == "ROBOT_START_TURNING_ANGLE")
            key_values.emplace(k, std::to_string(mSettings_->motor_angle_start));
        else if (k == "ROBOT_END_TURNING_ANGLE")
            key_values.emplace(k, std::to_string(mSettings_->motor_angle_end));
    }
    return key_values;
}

double iim_ego::execution::HeadControl::factor = 11.37777777777777;
uint16_t iim_ego::execution::HeadControl::center_value = 2048;
uint16_t iim_ego::execution::HeadControl::center_position_value = 2048;
uint16_t iim_ego::execution::HeadControl::right_position_value = 2048;
uint16_t iim_ego::execution::HeadControl::left_position_value = 2048;

iim_ego::execution::HeadControl::HeadControl()
    : TimeoutLimit(boost::chrono::seconds(3)),
      thd_turn_back_center(nullptr)
{
    if (mSettings_ != nullptr)
        delete mSettings_;
//     std::string file  = core::SystemConfig::getConstPtrInstance()->getRuntimePath() + "/execution/head_control/head_control.xml";
    mSettings_ = new HeadControlSettings();
//     core::SystemConfig::configurationLog() << "==>head-control settings\n" << *mSettings_ << std::endl;
    config_motor_with_settings();
}

iim_ego::execution::HeadControl::~HeadControl()
{
    hasFace = true;

    if (mSettings_ != nullptr) {
        if (mSettings_->bMotorStopWithFace) {
            thd_turn_back_center->interrupt();
            thd_turn_back_center->join();
            delete thd_turn_back_center;
        }
        delete mSettings_;
        mSettings_ = nullptr;
    }
}

void iim_ego::execution::HeadControl::config_motor_with_settings()
{
    transform_angle_to_value(std::cout);

    /* flag indicate the status and position of head. 0: center, -1: left, 1 right
       -2:center -> left, right -> center, 2: left -> center, center -> right */
    if (mSettings_->motor_turn_priority == "right")
        previous_turn_direction = true;
    else if (mSettings_->motor_turn_priority == "left")
        previous_turn_direction = false;
    nTurnDirection = !previous_turn_direction;

    nMoveStatus = stop_center;
    hasFace = true;
    bReturnCenter = false;

    if (mSettings_->bMotorStopWithFace) {
        thd_turn_back_center = new boost::thread(boost::bind(&HeadControl::turn_back_center_without_face, this));
//         basis::utils::setThreadName(thd_turn_back_center, "head-reseter");
    }

}


void iim_ego::execution::HeadControl::transform_angle_to_value(std::ostream& os)
{
    center_position_value = center_value + mSettings_->motor_angle_center * factor;
    right_position_value = center_value + mSettings_->motor_angle_end * factor;
    left_position_value = center_value + mSettings_->motor_angle_start * factor;
    os << "In HeadControl, right_position_value = " << right_position_value
       << ", center_position_value = " << center_position_value
       << ", left_position_value = "  << left_position_value << std::endl;
}

void iim_ego::execution::HeadControl::turn_back_center_without_face()
{
    std::cout << "start turn_back_center thread ..." << std::endl;
    try {
        bool bRet = false;
        while (true) {
            boost::this_thread::interruption_point();
            boost::mutex::scoped_lock lock(mtx_);
            bRet = cv_turn_back.timed_wait(lock, boost::get_system_time() + boost::posix_time::seconds(3));
            if (!bRet && hasFace) {
                hasFace = false;
                if (nMoveStatus == stop_somewhere || nMoveStatus == stop_left || nMoveStatus == stop_right) {
                    turn_to_center();
                    nMoveStatus = stop_center;
                    mSettings_->bMotorTurnOver = false;
                }
            }
        }
    } catch (boost::thread_interrupted&) {
        std::cout << "turn_back_center thread terminated ..." << std::endl;
    }
}


void iim_ego::execution::HeadControl::parse_sideviewInfo(perception::detection::RectVectorVectorConstPtr const& data)
{
    if (data->size() == 1) {
        if (mSettings_->onAnalysis_cam_left)
            nTurnDirection = true;
        else if (mSettings_->onAnalysis_cam_right)
            nTurnDirection = false;
        return;
    }

    if (data->at(0).size() > 0 && data->at(1).size() > 0) {
        if (mSettings_->motor_turn_priority == "right" || mSettings_->motor_turn_priority == "left") {
            if (previous_turn_direction) {
                nTurnDirection = false;
            } else {
                nTurnDirection = true;
            }
        } else {
            // priority is same, chose the left and right random, 0: left, 1: right
            srand(time(NULL));
            if (rand() % 2 == 1) {
                nTurnDirection = true;
            } else {
                nTurnDirection = false;
            }
        }
    } else if (data->at(0).size() > 0 && data->at(1).size() == 0) {
        nTurnDirection = true;
    } else if (data->at(0).size() == 0 && data->at(1).size() > 0) {
        nTurnDirection = false;
    }
}



void iim_ego::execution::HeadControl::onControl(bool hasFace,  perception::detection::RectVectorVectorConstPtr const& side_data)
{
    if (!bMotorInitSuccess) return;
    this->hasFace = hasFace;
    if (!hasFace)
        parse_sideviewInfo(side_data);
    else {
        if (mSettings_->bMotorStopWithFace/* && nMoveStatus == moving*/) {
            boost::lock_guard<boost::mutex> guard(mtx_);
            bool isCenter;
            if (stop_move(isCenter)) {
                if (isCenter) {
                    nMoveStatus = stop_center;
                    mSettings_->bMotorTurnOver = false;
                } else {
                    nMoveStatus = stop_somewhere;
                    mSettings_->bMotorTurnOver = true;
                }
            }
            cv_turn_back.notify_one();
        }
    }
}

bool iim_ego::execution::HeadControl::onMotor()
{
    if (bReturnCenter) {
        return false;
    }

    if (mSettings_->bMotorStopWithFace && hasFace)
        return false;
    if (!mSettings_->onAnalysis_cam_left && !mSettings_->onAnalysis_cam_right && (nMoveStatus == stop_center))
        return false;
    boost::unique_lock<boost::mutex> try_lock(mtx_, boost::try_to_lock);
    if (try_lock.owns_lock()) {
        if (nMoveStatus == stop_center && nTurnDirection == previous_turn_direction) {
            bool isCenter;
            if (!stop_move(isCenter)) {
                try_lock.unlock();
                return false;
            }
            if (isCenter) {
                nMoveStatus = stop_center;
                mSettings_->bMotorTurnOver = false;
            } else {
                if (previous_turn_direction)
                    nMoveStatus = stop_left;
                else
                    nMoveStatus = stop_right;
                mSettings_->bMotorTurnOver = true;
            }
        }
        if (!hasFace) {
            if (nTurnDirection) {
                turn_to_left();
                nMoveStatus = moving;
            } else {
                turn_to_right();
                nMoveStatus = moving;
            }
            previous_turn_direction = nTurnDirection;
            mSettings_->bMotorTurnOver = true;
            while (nMoveStatus == moving) {
                nMoveStatus = moving_status();
                boost::this_thread::sleep_for(boost::chrono::milliseconds(150));
            }
        }

        if (nMoveStatus == stop_left || nMoveStatus == stop_right || nMoveStatus == stop_somewhere) {
            boost::this_thread::sleep_for(TimeoutLimit);//keep status 3 seconds
            bReturnCenter = true;
            turn2CenterWithDelay();
            nMoveStatus = stop_center;
            mSettings_->bMotorTurnOver = false;
        } else {
            bReturnCenter = false;
        }
        try_lock.unlock();
    } else {
        std::cout << "Operate too fast!" << std::endl;
    }

    return bReturnCenter;
}





