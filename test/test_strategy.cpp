#include "strategy.h"
#include "types.h"
#include <iostream>
#include <chrono>
#include <iomanip>

// 打印时间戳的辅助函数
void print_timestamp(const std::chrono::system_clock::time_point& time) {
    auto time_t = std::chrono::system_clock::to_time_t(time);
    std::cout << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
}

// 打印信号的辅助函数
void print_signal(const Signal& signal) {
    std::cout << "Signal: ";
    switch (signal.type) {
        case SignalType::BUY:
            std::cout << "BUY";
            break;
        case SignalType::SELL:
            std::cout << "SELL";
            break;
        case SignalType::HOLD:
            std::cout << "HOLD";
            break;
    }
    std::cout << " at price: " << signal.price << " on ";
    print_timestamp(signal.timestamp);
    std::cout << std::endl;
}

int main() {
    // 创建测试数据 - 模拟一个先涨后跌的价格走势
    std::vector<Bar> bars;
    auto base_time = std::chrono::system_clock::now();
    
    // 创建15个数据点
    double prices[] = {100, 102, 105, 108, 110, 115, 120, 118, 115, 112, 110, 108, 105, 103, 100};
    
    for (int i = 0; i < 15; ++i) {
        Bar bar;
        bar.timestamp = base_time + std::chrono::hours(24 * i);
        bar.closePrice = prices[i];
        bars.push_back(bar);
    }
    
    // 创建移动平均线交叉策略（5日均线和10日均线）
    MovingAverageCrossStrategy strategy(5, 10);
    
    // 生成信号
    auto signals = strategy.generate_signals(bars);
    
    // 打印结果
    std::cout << "Strategy: " << strategy.get_name() << std::endl;
    std::cout << "Generated signals:" << std::endl;
    std::cout << "==================" << std::endl;
    
    if (signals.empty()) {
        std::cout << "No signals generated." << std::endl;
    } else {
        for (const auto& signal : signals) {
            print_signal(signal);
        }
    }
    
    return 0;
}
