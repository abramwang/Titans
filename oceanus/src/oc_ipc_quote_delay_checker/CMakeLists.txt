cmake_minimum_required(VERSION 2.8)

project(oc_ipc_quote_delay_checker)


# This ensures that things like gnu++11 get passed correctly
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "-std=c++17 -pthread")
# We require a C++11 compliant compiler
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

# suport GDB
#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_BUILD_TYPE "Release")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

file(GLOB SRCS *.c *.cpp *.cc *.h *.hpp )

aux_source_directory("../../../resource/src" SRCS)

include_directories(
    "../../../resource/include"
    "../../../resource/src"
)

link_directories( "./" )


add_executable( ${PROJECT_NAME} ${SRCS} )

target_link_libraries(${PROJECT_NAME} -lcurl -lmariadb -larrow -lparquet -luv -lhiredis -lglog -lrt)


# 指定源文件和目标文件路径
set(SOURCE_FILE "./oc_ipc_quote_demo")
set(TARGET_DIR "/home/dist/production_env")

# 添加自定义命令
add_custom_command(
    TARGET oc_ipc_quote_demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_FILE} ${TARGET_DIR}
)