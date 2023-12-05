cmake_minimum_required(VERSION 3.16)

project(advent_of_code VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost 1.80 REQUIRED)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
endif()

find_package(Eigen3 3.3 REQUIRED)

add_executable(advent_of_code
    src/main.cpp
    src/util.cpp
    src/2023/day_01.cpp
    src/2023/day_02.cpp
    src/2023/day_03.cpp
    src/2023/day_04.cpp
    src/2023/day_05.cpp
    src/2023/day_06.cpp
    src/2023/day_07.cpp
    src/2023/day_08.cpp
    src/2023/day_09.cpp
    src/2023/day_10.cpp
    src/2023/day_11.cpp
    src/2023/day_12.cpp
    src/2023/day_13.cpp
    src/2023/day_14.cpp
    src/2023/day_15.cpp
    src/2023/day_16.cpp
    src/2023/day_17.cpp
    src/2023/day_18.cpp
    src/2023/day_19.cpp
    src/2023/day_20.cpp
    src/2023/day_21.cpp
    src/2023/day_22.cpp
    src/2023/day_23.cpp
    src/2023/day_24.cpp
    src/2023/day_25.cpp
  )

target_link_libraries(advent_of_code PRIVATE Eigen3::Eigen)

