cmake_minimum_required(VERSION 3.10)
project(StockBacktrack)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -pthread")

include_directories(include)

add_executable(stock_backtrack
    src/types.cpp
    src/data_handler.cpp
    src/main.cpp
    src/thread_pool.cpp
)

add_executable(test_thread_pool
    src/thread_pool.cpp
    test_thread_pool.cpp
)

add_executable(test_indicator
    src/indicator.cpp
    test_indicator.cpp
)