cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(emt_api_demo)

if (MSVC)
    link_directories(${PROJECT_SOURCE_DIR}/../lib/windows/x64)

else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	link_directories(${PROJECT_SOURCE_DIR}/../lib/linux)
endif()


find_package(Threads)
# 将源文件目录中的文件排除 quote_login_demo_lv2.cpp
file(GLOB DEMO_SRCS ${PROJECT_SOURCE_DIR}/*.cpp)
list(REMOVE_ITEM DEMO_SRCS ${PROJECT_SOURCE_DIR}/quote_login_demo_lv2.cpp)

add_executable(${PROJECT_NAME} ${DEMO_SRCS})

target_include_directories(${PROJECT_NAME}
	PRIVATE ${PROJECT_SOURCE_DIR}/../include
)
target_link_libraries(${PROJECT_NAME}
	emt_api emt_quote_api emt_quote_api_lv2 ${CMAKE_THREAD_LIBS_INIT}
)
if (MSVC)

    target_compile_options(${PROJECT_NAME}
        PRIVATE
            "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
            "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>"
    )

endif()

# 添加行情Lv2 Api 登录demo
add_executable(quote_login_demo_lv2 quote_login_demo_lv2.cpp)
target_include_directories(quote_login_demo_lv2
		PRIVATE ${PROJECT_SOURCE_DIR}/../include
)
target_link_libraries(quote_login_demo_lv2 emt_quote_api_lv2 ${CMAKE_THREAD_LIBS_INIT})