cmake_minimum_required(VERSION 3.17)
project(EntSnap)

find_package(raylib 2.5.0 REQUIRED) # Requires at least version 2.5.0

set(CMAKE_CXX_STANDARD 20)

include_directories(src include)

file(GLOB_RECURSE FILES
        "src/*.hpp"
        "src/*.cpp"
        )

add_executable(EntSnap ${FILES} src/Project.hpp src/Project.cpp src/Component.hpp src/Entity.hpp)

target_link_libraries(EntSnap raylib)
