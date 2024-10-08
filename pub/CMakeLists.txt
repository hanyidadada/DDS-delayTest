# 发布者子项目
project(publisher)

# 包含头文件目录
include_directories(
    ${PROJECT_SOURCE_DIR}/../src
    ${PROJECT_SOURCE_DIR}/../idl
)

# 查找 Fast DDS 和 Fast CDR 库
find_package(fastrtps REQUIRED)
find_package(fastcdr REQUIRED)
find_package(Threads REQUIRED)

# 发布者源文件
set(GENERATED_SOURCES
    ${PROJECT_SOURCE_DIR}/../src/TimeTestPub.cxx
    ${PROJECT_SOURCE_DIR}/../src/TimeTestPubPubSubTypes.cxx
    ${PROJECT_SOURCE_DIR}/../src/TimeTestSub.cxx
    ${PROJECT_SOURCE_DIR}/../src/TimeTestSubPubSubTypes.cxx
)

set(PUB_SOURCES
    ${PROJECT_SOURCE_DIR}/publisher.cpp
    ${GENERATED_SOURCES}
)
# 生成可执行文件
add_executable(publisher ${PUB_SOURCES})

# 链接库
target_link_libraries(publisher fastrtps fastcdr Threads::Threads)
