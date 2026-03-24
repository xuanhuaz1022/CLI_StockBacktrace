#include "execution_handler.h"
#include "strategy.h"
#include "types.h"
#include <iostream>
#include <chrono>

int main()
{
    // 创建初始资金为10000元的执行处理器
    ExecutionHandler executor(10000.0);

    // 创建测试数据 - 包含明显的均线交叉
    std::vector<Bar> bars;
    auto base_time = std::chrono::system_clock::now();

    // 创建10个数据点，模拟先涨后跌的走势
    double prices[] = {100, 102, 105, 108, 110, 115, 120, 118, 115, 112};

    for (int i = 0; i < 10; ++i)
    {
        Bar bar;
        bar.timestamp = base_time + std::chrono::hours(24 * i);
        bar.closePrice = prices[i];
        bars.push_back(bar);
    }

    // 创建策略 (使用更灵敏的参数组合)
    MovingAverageCrossStrategy strategy(3, 5);

    // 生成信号
    auto signals = strategy.generate_signals(bars, "TEST_STOCK");

    // 执行信号前的投资组合状态
    std::cout << "执行前:" << std::endl;
    std::cout << "现金: " << executor.get_cash() << " 元" << std::endl;
    std::cout << "总价值: " << executor.get_total_value() << " 元" << std::endl;
    std::cout << "持仓数量: " << executor.get_positions().size() << std::endl;
    std::cout << std::endl;

    // 执行信号
    executor.execute_signals(signals, bars);

    // 执行信号后的投资组合状态
    std::cout << "执行后:" << std::endl;
    std::cout << "现金: " << executor.get_cash() << " 元" << std::endl;
    std::cout << "总价值: " << executor.get_total_value() << " 元" << std::endl;
    std::cout << "持仓数量: " << executor.get_positions().size() << std::endl;

    // 打印持仓详情
    const auto &positions = executor.get_positions();
    for (const auto &pos : positions)
    {
        std::cout << "股票: " << pos.symbol << ", 数量: " << pos.quantity
                  << ", 平均成本: " << pos.average_price << std::endl;
    }

    return 0;
}
