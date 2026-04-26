cmake_minimum_required(VERSION 3.10.2)
project("sys_core")

# main.cpp 하나만 빌드하면 그 안에서 다른 파일들을 모두 가져와 컴파일합니다.
add_library(sys_core SHARED main.cpp)

find_library(log-lib log)
target_link_libraries(sys_core ${log-lib} android)
