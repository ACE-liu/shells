#include <string>
#include <boost/thread.hpp>
#include <boost/chrono/duration.hpp>
#include "dynamixel_head_control.h"
#include "head_control_factory.h"
#include "type_detection.h"
#include <boost/progress.hpp>

int main(int argc, char* argv[])
{
    //if(!iim_ego::execution::DynamixelHeadControl::resetDynamixelID())
    //    exit(-1);
    perception::detection::RectVectorVectorPtr detection_object_ptr(new perception::detection::RectVectorVector);
    perception::detection::RectVector object_vector;
    object_vector.push_back(cv::Rect());
    detection_object_ptr->push_back(object_vector);
//     iim_ego::execution::HeadControl* head_control =iim_ego::execution::headControlFactory::createInstance();
    iim_ego::execution::HeadControl* head_control =new iim_ego::execution::DynamixelHeadControl();
    head_control->getParam().onAnalysis_cam_left = false;
    head_control->getParam().onAnalysis_cam_right = true;

    for(int i = 0; i < 1; i++)
    {
        boost::progress_timer t1;
        head_control->onControl(false, detection_object_ptr);
        while (head_control->onMotor()) {
        }
        std::cout << t1.elapsed() << std::endl;
        boost::this_thread::sleep_for(boost::chrono::seconds(5));
    }
    boost::this_thread::sleep_for(boost::chrono::seconds(4));

    head_control->getParam().onAnalysis_cam_left = true;
    head_control->getParam().onAnalysis_cam_right = false;
    for(int i = 0; i < 1; i++)
    {
        boost::progress_timer t1;
        head_control->onControl(false, detection_object_ptr);
        while (head_control->onMotor()) {
        }
        std::cout << t1.elapsed() << std::endl;
        boost::this_thread::sleep_for(boost::chrono::seconds(8));
    }
//     detection_object_ptr->push_back(object_vector);
//     head_control->getParam().onAnalysis_cam_left = true;
//     head_control->getParam().onAnalysis_cam_right = true;

//     for(int i = 0; i < 3; i++)
//     {
//         boost::progress_timer t1;
//         head_control->onControl(false, detection_object_ptr);
//         while (head_control->onMotor()) {
//         }
//         std::cout << t1.elapsed() << std::endl;
//         boost::this_thread::sleep_for(boost::chrono::seconds(5));
//         boost::progress_timer t;
//         head_control->onControl(true, detection_object_ptr);
//         std::cout << t.elapsed() << std::endl;
//         boost::this_thread::sleep_for(boost::chrono::seconds(15));
//     }
    
    delete head_control;
}

