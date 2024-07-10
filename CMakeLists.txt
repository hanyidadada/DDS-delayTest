cmake_minimum_required(VERSION 3.5)
project(FastDDSTest)
# Find requirements
if(NOT fastcdr_FOUND)
    find_package(fastcdr 2 REQUIRED)
endif()

if(NOT fastrtps_FOUND)
    find_package(fastrtps 2.12 REQUIRED)
endif()

#Check C++11
include(CheckCXXCompilerFlag)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    check_cxx_compiler_flag(-std=c++11 SUPPORTS_CXX11)
    if(NOT SUPPORTS_CXX11)
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

set(CMAKE_CXX_STANDARD 11)


file(GLOB DDS_CUSTOMLISTENER_EXAMPLE_SOURCES_CXX "*.cxx")

add_executable(publisher publisher.cpp ${DDS_CUSTOMLISTENER_EXAMPLE_SOURCES_CXX})
add_executable(subscriber subscriber.cpp ${DDS_CUSTOMLISTENER_EXAMPLE_SOURCES_CXX})

target_link_libraries(publisher fastcdr fastrtps)
target_link_libraries(subscriber fastcdr fastrtps)
