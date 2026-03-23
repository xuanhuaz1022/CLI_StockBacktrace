cmake_minimum_required(VERSION 3.10)
project(StockBacktrack)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -pthread")

# 查找SQLite3依赖
find_package(SQLite3 REQUIRED)

# 查找nlohmann/json依赖
find_package(nlohmann_json REQUIRED)

# 查找Drogon依赖
find_package(Drogon REQUIRED)

# 查找CLI11依赖
find_package(CLI11 REQUIRED)

include_directories(core/include)
include_directories(backend)

add_executable(stock_backtrack
    core/src/types.cpp
    core/src/data_handler.cpp
    core/src/indicator.cpp
    core/src/strategy.cpp
    core/src/execution_handler.cpp
    core/src/backtest_engine.cpp
    core/src/main.cpp
    core/src/thread_pool.cpp
    backend/models/Database.cpp
    backend/cli/CLI.cpp
)

target_link_libraries(stock_backtrack
    PRIVATE SQLite::SQLite3
    PRIVATE nlohmann_json::nlohmann_json
    PRIVATE Drogon::Drogon
    PRIVATE CLI11::CLI11
)

add_executable(test_thread_pool
    core/src/thread_pool.cpp
    test/test_thread_pool.cpp
)

add_executable(test_indicator
    core/src/indicator.cpp
    test/test_indicator.cpp
)

add_executable(test_strategy
    core/src/indicator.cpp
    core/src/strategy.cpp
    test/test_strategy.cpp
)

add_executable(test_execution
    core/src/types.cpp
    core/src/indicator.cpp
    core/src/strategy.cpp
    core/src/execution_handler.cpp
    test/test_execution.cpp
)

add_executable(test_execution_direct
    core/src/types.cpp
    core/src/execution_handler.cpp
    test/test_execution_direct.cpp
)

add_executable(test_backtest
    core/src/types.cpp
    core/src/data_handler.cpp
    core/src/indicator.cpp
    core/src/strategy.cpp
    core/src/execution_handler.cpp
    core/src/backtest_engine.cpp
    test/test_backtest.cpp
)

# 链接测试程序的依赖库
target_link_libraries(test_thread_pool
    PRIVATE nlohmann_json::nlohmann_json
)

target_link_libraries(test_indicator
    PRIVATE nlohmann_json::nlohmann_json
)

target_link_libraries(test_strategy
    PRIVATE nlohmann_json::nlohmann_json
)

target_link_libraries(test_execution
    PRIVATE nlohmann_json::nlohmann_json
)

target_link_libraries(test_execution_direct
    PRIVATE nlohmann_json::nlohmann_json
)

target_link_libraries(test_backtest
    PRIVATE SQLite::SQLite3
    PRIVATE nlohmann_json::nlohmann_json
)