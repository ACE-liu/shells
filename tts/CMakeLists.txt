cmake_minimum_required (VERSION 2.8)

# projectname is the same as the main-executable
project(tts)

add_definitions('-g')
add_definitions('-Wall')
add_definitions('-std=c++11')
add_definitions(-DWORK_DIR="${CMAKE_SOURCE_DIR}")
include_directories(include/)
link_directories(${CMAKE_SOURCE_DIR}/lib/)
file(GLOB srcs src/*.cpp)
add_executable(${PROJECT_NAME} main.cpp ${srcs})
target_link_libraries(${PROJECT_NAME} msc)

