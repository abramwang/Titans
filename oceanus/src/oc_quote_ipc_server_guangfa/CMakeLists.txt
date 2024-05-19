cmake_minimum_required(VERSION 2.8)

project(oc_quote_ipc_server_guangfa)

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

# 复制依赖so文件 到编译目录
file(GLOB allCopyFiles  "../../../resource/drivers/guangfa/lib/*")
file(COPY ${allCopyFiles} DESTINATION "./")

aux_source_directory("../../../resource/src" SRCS)
aux_source_directory("../../../resource/drivers/guangfa/src" SRCS)

include_directories(
    "../../../resource/include"
    "../../../resource/src"
    "../../../resource/drivers/guangfa/include"
    "../../../resource/drivers/guangfa/src"
)

link_directories( "./" )


add_executable( ${PROJECT_NAME} ${SRCS} )

target_link_libraries(${PROJECT_NAME} -lama -latptradeapi -lcurl -larrow -lparquet -luv -lhiredis -lmysqlclient -lglog -lrt)