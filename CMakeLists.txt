# CMake 最低版本要求
cmake_minimum_required(VERSION 3.5)

# 项目名称
project(TimeTestProject)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加子目录
add_subdirectory(pub)
add_subdirectory(sub)