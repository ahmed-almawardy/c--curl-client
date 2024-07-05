cmake_minimum_required(VERSION 3.28)
project(api_client)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -lcurl")

add_executable(
        api_client main.cpp
        FormMultiPart.cpp
        FormMultiPart.h
)

target_link_libraries(api_client curl)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
