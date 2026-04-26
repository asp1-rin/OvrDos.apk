cmake_minimum_required(VERSION 3.10.2)
project("sys_core")

add_library(
    sys_core
    SHARED
    memory.cpp
    aimbot.cpp
    bypass.cpp
    main.cpp
)

find_library(log-lib log)
target_link_libraries(sys_core ${log-lib} android)
