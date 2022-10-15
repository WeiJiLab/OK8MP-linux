cmake_minimum_required(VERSION 3.5)
project(ml-security-test VERSION 1.0.0 LANGUAGES CXX)

add_executable(ax_sample ax_sample.cpp)

find_package(ml-security 1.0 REQUIRED)
target_link_libraries(ax_sample ${ml-security_LIBS})

find_package(OpenCV REQUIRED)
target_link_libraries(ax_sample ${OpenCV_LIBS})

add_executable(cloning_sample cloning_sample.cpp)
target_link_libraries(cloning_sample ${ml-security_LIBS})
target_link_libraries(cloning_sample ${OpenCV_LIBS})
