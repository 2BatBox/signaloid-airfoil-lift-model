cmake_minimum_required(VERSION 3.10)
project(stardust VERSION 0.4.0 DESCRIPTION "signalod-airfoil-lift-model")

set(PROJECT_NAME "demo")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3 ")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fomit-frame-pointer -fvisibility=hidden -s -fomit-frame-pointer")

set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} ")
#set(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")

set(APP "app")
add_executable(${APP} src/main.c)
target_link_libraries(${APP} m)
