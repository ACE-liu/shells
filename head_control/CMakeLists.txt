cmake_minimum_required(VERSION 2.8.3)
set(prj_name "head_control")
project(${prj_name})

add_compile_options(-std=c++11)

set(head_libs "dxl_x64_cpp;pthread")

# if(core_FOUND)
#     INCLUDE_DIRECTORIES(${core_INCLUDE_DIRS})
#     list(APPEND head_libs ${core_LIBRARIES})
# endif(core_FOUND)
find_package(Boost 1.61.0 REQUIRED COMPONENTS thread chrono filesystem system date_time)
if(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    list(APPEND head_libs ${Boost_LIBRARIES})
endif(Boost_FOUND)

find_package(OpenCV 3.3.1 REQUIRED)
if(OpenCV_FOUND)
    INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIRS})
    list(APPEND head_libs ${OpenCV_LIBRARIES})
endif(OpenCV_FOUND)

find_package(Glog)
if(GLOG_FOUND)
    INCLUDE_DIRECTORIES(${GLOG_INCLUDE_DIRS})
    list(APPEND head_libs ${GLOG_LIBRARIES})
endif(GLOG_FOUND)

include_directories(include)

link_directories(lib)

file(GLOB hdrs include/*.*)

add_library(${prj_name}
  head_control.cpp
  dynamixel_head_control.cpp
  smsbl_motor.cpp
  smsbl_head_control.cpp
  head_control_factory.cpp
  head_control_settings.cpp
  ${hdrs})
target_link_libraries(${prj_name} ${head_libs})

add_executable(${prj_name}Test main_head_control.cpp)

target_link_libraries(${prj_name}Test ${head_libs} ${prj_name})


