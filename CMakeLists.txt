cmake_minimum_required(VERSION 3.20)

project(advent_of_code VERSION 1.0.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- 1. Find Boost ---
find_package(Boost CONFIG REQUIRED)

# --- 2. Find Eigen ---
find_package(Eigen3 3.3 REQUIRED)

# --- 3. Find Coin-OR Libraries via PkgConfig ---
find_package(PkgConfig REQUIRED)

# We need ALL of these modules to resolve the symbols:
# - cbc: The main solver (CbcModel)
# - osi-clp: The interface (OsiClpSolverInterface)
# - coinutils: The data structures (CoinPackedMatrix)
pkg_check_modules(COIN_DEPS REQUIRED 
    cbc 
    osi-clp 
    osi
    coinutils
    IMPORTED_TARGET
)

# --- 2. Define Executable (Create the target FIRST) ---
add_executable(advent_of_code
    src/main.cpp
    src/util/util.cpp
    src/util/make_year.cpp

    src/2025/day_01.cpp
    src/2025/day_02.cpp
    src/2025/day_03.cpp
    src/2025/day_04.cpp
    src/2025/day_05.cpp
    src/2025/day_06.cpp
    src/2025/day_07.cpp
    src/2025/day_08.cpp
    src/2025/day_09.cpp
    src/2025/day_10.cpp
    src/2025/day_11.cpp
    src/2025/day_12.cpp
    src/2025/y2025.cpp

    src/2024/day_01.cpp
    src/2024/day_02.cpp
    src/2024/day_03.cpp
    src/2024/day_04.cpp
    src/2024/day_05.cpp
    src/2024/day_06.cpp
    src/2024/day_07.cpp
    src/2024/day_08.cpp
    src/2024/day_09.cpp
    src/2024/day_10.cpp
    src/2024/day_11.cpp
    src/2024/day_12.cpp
    src/2024/day_13.cpp
    src/2024/day_14.cpp
    src/2024/day_15.cpp
    src/2024/day_16.cpp
    src/2024/day_17.cpp
    src/2024/day_18.cpp
    src/2024/day_19.cpp
    src/2024/day_20.cpp
    src/2024/day_21.cpp
    src/2024/day_22.cpp
    src/2024/day_23.cpp
    src/2024/day_24.cpp
    src/2024/day_25.cpp
    src/2024/y2024.cpp

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
    src/2023/y2023.cpp

    src/2022/day_01.cpp
    src/2022/day_02.cpp
    src/2022/day_03.cpp
    src/2022/day_04.cpp
    src/2022/day_05.cpp
    src/2022/day_06.cpp
    src/2022/day_07.cpp
    src/2022/day_08.cpp
    src/2022/day_09.cpp
    src/2022/day_10.cpp
    src/2022/day_11.cpp
    src/2022/day_12.cpp
    src/2022/day_13.cpp
    src/2022/day_14.cpp
    src/2022/day_15.cpp
    src/2022/day_16.cpp
    src/2022/day_17.cpp
    src/2022/day_18.cpp
    src/2022/day_19.cpp
    src/2022/day_20.cpp
    src/2022/day_21.cpp
    src/2022/day_22.cpp
    src/2022/day_23.cpp
    src/2022/day_24.cpp
    src/2022/day_25.cpp
    src/2022/y2022.cpp

    src/2021/day_01.cpp
    src/2021/day_02.cpp
    src/2021/day_03.cpp
    src/2021/day_04.cpp
    src/2021/day_05.cpp
    src/2021/day_06.cpp
    src/2021/day_07.cpp
    src/2021/day_08.cpp
    src/2021/day_09.cpp
    src/2021/day_10.cpp
    src/2021/day_11.cpp
    src/2021/day_12.cpp
    src/2021/day_13.cpp
    src/2021/day_14.cpp
    src/2021/day_15.cpp
    src/2021/day_16.cpp
    src/2021/day_17.cpp
    src/2021/day_18.cpp
    src/2021/day_19.cpp
    src/2021/day_20.cpp
    src/2021/day_21.cpp
    src/2021/day_22.cpp
    src/2021/day_23.cpp
    src/2021/day_24.cpp
    src/2021/day_25.cpp
    src/2021/y2021.cpp

    src/2020/day_01.cpp
    src/2020/day_02.cpp
    src/2020/day_03.cpp
    src/2020/day_04.cpp
    src/2020/day_05.cpp
    src/2020/day_06.cpp
    src/2020/day_07.cpp
    src/2020/day_08.cpp
    src/2020/day_09.cpp
    src/2020/day_10.cpp
    src/2020/day_11.cpp
    src/2020/day_12.cpp
    src/2020/day_13.cpp
    src/2020/day_14.cpp
    src/2020/day_15.cpp
    src/2020/day_16.cpp
    src/2020/day_17.cpp
    src/2020/day_18.cpp
    src/2020/day_19.cpp
    src/2020/day_20.cpp
    src/2020/day_21.cpp
    src/2020/day_22.cpp
    src/2020/day_23.cpp
    src/2020/day_24.cpp
    src/2020/day_25.cpp
    src/2020/y2020.cpp

    src/2019/day_01.cpp
    src/2019/day_02.cpp
    src/2019/day_03.cpp
    src/2019/day_04.cpp
    src/2019/day_05.cpp
    src/2019/day_06.cpp
    src/2019/day_07.cpp
    src/2019/day_08.cpp
    src/2019/day_09.cpp
    src/2019/day_10.cpp
    src/2019/day_11.cpp
    src/2019/day_12.cpp
    src/2019/day_13.cpp
    src/2019/day_14.cpp
    src/2019/day_15.cpp
    src/2019/day_16.cpp
    src/2019/day_17.cpp
    src/2019/day_18.cpp
    src/2019/day_19.cpp
    src/2019/day_20.cpp
    src/2019/day_21.cpp
    src/2019/day_22.cpp
    src/2019/day_23.cpp
    src/2019/day_24.cpp
    src/2019/day_25.cpp
    src/2019/y2019.cpp
    src/2019/intcode.cpp

    src/2018/day_01.cpp
    src/2018/day_02.cpp
    src/2018/day_03.cpp
    src/2018/day_04.cpp
    src/2018/day_05.cpp
    src/2018/day_06.cpp
    src/2018/day_07.cpp
    src/2018/day_08.cpp
    src/2018/day_09.cpp
    src/2018/day_10.cpp
    src/2018/day_11.cpp
    src/2018/day_12.cpp
    src/2018/day_13.cpp
    src/2018/day_14.cpp
    src/2018/day_15.cpp
    src/2018/day_16.cpp
    src/2018/day_17.cpp
    src/2018/day_18.cpp
    src/2018/day_19.cpp
    src/2018/day_20.cpp
    src/2018/day_21.cpp
    src/2018/day_22.cpp
    src/2018/day_23.cpp
    src/2018/day_24.cpp
    src/2018/day_25.cpp
    src/2018/y2018.cpp

    src/2017/day_01.cpp
    src/2017/day_02.cpp
    src/2017/day_03.cpp
    src/2017/day_04.cpp
    src/2017/day_05.cpp
    src/2017/day_06.cpp
    src/2017/day_07.cpp
    src/2017/day_08.cpp
    src/2017/day_09.cpp
    src/2017/day_10.cpp
    src/2017/day_11.cpp
    src/2017/day_12.cpp
    src/2017/day_13.cpp
    src/2017/day_14.cpp
    src/2017/day_15.cpp
    src/2017/day_16.cpp
    src/2017/day_17.cpp
    src/2017/day_18.cpp
    src/2017/day_19.cpp
    src/2017/day_20.cpp
    src/2017/day_21.cpp
    src/2017/day_22.cpp
    src/2017/day_23.cpp
    src/2017/day_24.cpp
    src/2017/day_25.cpp
    src/2017/knot_hash.cpp
    src/2017/y2017.cpp

    src/2016/day_01.cpp
    src/2016/day_02.cpp
    src/2016/day_03.cpp
    src/2016/day_04.cpp
    src/2016/day_05.cpp
    src/2016/day_06.cpp
    src/2016/day_07.cpp
    src/2016/day_08.cpp
    src/2016/day_09.cpp
    src/2016/day_10.cpp
    src/2016/day_11.cpp
    src/2016/day_12.cpp
    src/2016/day_13.cpp
    src/2016/day_14.cpp
    src/2016/day_15.cpp
    src/2016/day_16.cpp
    src/2016/day_17.cpp
    src/2016/day_18.cpp
    src/2016/day_19.cpp
    src/2016/day_20.cpp
    src/2016/day_21.cpp
    src/2016/day_22.cpp
    src/2016/day_23.cpp
    src/2016/day_24.cpp
    src/2016/day_25.cpp
    src/2016/y2016.cpp

    src/2015/day_01.cpp
    src/2015/day_02.cpp
    src/2015/day_03.cpp
    src/2015/day_04.cpp
    src/2015/day_05.cpp
    src/2015/day_06.cpp
    src/2015/day_07.cpp
    src/2015/day_08.cpp
    src/2015/day_09.cpp
    src/2015/day_10.cpp
    src/2015/day_11.cpp
    src/2015/day_12.cpp
    src/2015/day_13.cpp
    src/2015/day_14.cpp
    src/2015/day_15.cpp
    src/2015/day_16.cpp
    src/2015/day_17.cpp
    src/2015/day_18.cpp
    src/2015/day_19.cpp
    src/2015/day_20.cpp
    src/2015/day_21.cpp
    src/2015/day_22.cpp
    src/2015/day_23.cpp
    src/2015/day_24.cpp
    src/2015/day_25.cpp
    src/2015/y2015.cpp
    src/2015/md5.cpp
    src/2015/md5.h
)

# --- 5. Link Libraries ---
target_link_libraries(advent_of_code PRIVATE 
    Boost::headers
    Eigen3::Eigen
    PkgConfig::COIN_DEPS  # Links cbc, osi-clp, and coinutils
)