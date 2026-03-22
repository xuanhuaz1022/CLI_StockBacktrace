#include "backtest_engine.h"
#include "data_handler.h"
#include "strategy.h"
#include "execution_handler.h"
#include "types.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <ctime>

// 打印回测结果的辅助函数
void print_backtest_result(const BacktestResult& result) {
    std::cout << "========== 回测结果 ==========" << std::endl;
    std::cout << "策略名称: " << result.strategy_name << std::endl;
    std::cout << "初始资金: " << std::fixed << std::setprecision(2) << result.initial_equity << " 元" << std::endl;
    std::cout << "最终资金: " << std::fixed << std::setprecision(2) << result.final_equity << " 元" << std::endl;
    std::cout << "总收益率: " << std::fixed << std::setprecision(2) << (result.return_rate * 100) << "%" << std::endl;
    std::cout << "最大回撤: " << std::fixed << std::setprecision(2) << (result.max_drawdown * 100) << "%" << std::endl;
    std::cout << "夏普比率: " << std::fixed << std::setprecision(2) << result.sharpe_ratio << std::endl;
    std::cout << "===========================" << std::endl;
}

// 打印权益曲线
void print_equity_curve(const std::vector<double>& equity) {
    std::cout << "\n权益曲线:" << std::endl;
    std::cout << "------------------------" << std::endl;
    for (size_t i = 0; i < equity.size(); ++i) {
        std::cout << "Day " << i << ": " << std::fixed << std::setprecision(2) << equity[i] << " 元" << std::endl;
    }
}

int main() {
    // 创建数据处理器
    DataHandler data_handler;
    
    // 创建测试数据
    std::vector<Bar> bars;
    auto base_time = std::chrono::system_clock::now();
    
    // 创建20个数据点，模拟一个先涨后跌再涨的走势
    double prices[] = {
        100, 102, 105, 108, 110, 115, 120, 125, 122, 118,
        115, 110, 108, 112, 115, 120, 125, 130, 135, 140
    };
    
    // 创建一个临时的CSV文件来加载测试数据
    std::ofstream temp_file("test_data.csv");
    temp_file << "timestamp,open,high,low,close,volume" << std::endl;
    
    for (int i = 0; i < 20; ++i) {
        auto timestamp = base_time + std::chrono::hours(24 * i);
        time_t time_t_timestamp = std::chrono::system_clock::to_time_t(timestamp);
        struct tm* tm_time = localtime(&time_t_timestamp);
        
        char timestamp_str[20];
        strftime(timestamp_str, sizeof(timestamp_str), "%Y-%m-%d %H:%M:%S", tm_time);
        
        temp_file << timestamp_str << "," 
                 << prices[i] << "," 
                 << (prices[i] + 2) << "," 
                 << (prices[i] - 2) << "," 
                 << prices[i] << "," 
                 << 1000 << std::endl;
    }
    temp_file.close();
    
    // 使用DataHandler的公共接口加载数据
    data_handler.load_data("test_data.csv", "TEST_STOCK");
    
    // 删除临时文件
    remove("test_data.csv");
    
    // 创建策略（5日均线和10日均线交叉策略）
    MovingAverageCrossStrategy strategy(5, 10);
    
    // 创建执行处理器（初始资金10000元）
    ExecutionHandler executor(10000.0);
    
    // 创建回测引擎
    BacktestEngine engine(&data_handler, &strategy, &executor);
    
    // 运行回测
    auto start_time = base_time;
    auto end_time = base_time + std::chrono::hours(24 * 19);
    engine.run("TEST_STOCK", start_time, end_time);
    
    // 获取并打印回测结果
    const auto& result = engine.get_result();
    print_backtest_result(result);
    
    // 打印权益曲线
    print_equity_curve(engine.get_equity_curve());
    
    return 0;
}
