
#include "indicator.h"
#include "types.h"
#include <iostream>
#include <chrono>

int main() {
    // 创建测试数据
    std::vector<Bar> bars;
    
    // 添加10个测试数据点
    for (int i = 1; i <= 10; ++i) {
        Bar bar;
        bar.timestamp = std::chrono::system_clock::now();
        bar.closePrice = i * 10.0;  // 收盘价为10, 20, 30, ..., 100
        bars.push_back(bar);
    }
    
    // 测试SMA计算
    std::cout << "Testing SMA (period=3):" << std::endl;
    auto sma_result = simple_moving_average(bars, 3);
    for (size_t i = 0; i < sma_result.size(); ++i) {
        std::cout << "Day " << i+1 << ": " << sma_result[i] << std::endl;
    }
    
    // 测试EMA计算
    std::cout << "\nTesting EMA (period=3):" << std::endl;
    auto ema_result = calculate_ema(bars, 3);
    for (size_t i = 0; i < ema_result.size(); ++i) {
        std::cout << "Day " << i+1 << ": " << ema_result[i] << std::endl;
    }
    
    return 0;
}
