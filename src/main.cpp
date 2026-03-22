#include "data_handler.h"
#include "strategy.h"
#include "execution_handler.h"
#include "backtest_engine.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <string>

// 打印回测结果的函数
void print_backtest_result(const BacktestResult &result)
{
    std::cout << "========== 股票回测系统 ==========" << std::endl;
    std::cout << "策略名称: " << result.strategy_name << std::endl;
    std::cout << "初始资金: " << std::fixed << std::setprecision(2) << result.initial_equity << " 元" << std::endl;
    std::cout << "最终资金: " << std::fixed << std::setprecision(2) << result.final_equity << " 元" << std::endl;
    std::cout << "总收益率: " << std::fixed << std::setprecision(2) << (result.return_rate * 100) << "%" << std::endl;
    std::cout << "最大回撤: " << std::fixed << std::setprecision(2) << (result.max_drawdown * 100) << "%" << std::endl;
    std::cout << "夏普比率: " << std::fixed << std::setprecision(2) << result.sharpe_ratio << std::endl;
    std::cout << "=================================" << std::endl;
}

// 打印权益曲线
void print_equity_curve(const std::vector<double> &equity)
{
    std::cout << "\n权益曲线:" << std::endl;
    std::cout << "------------------------" << std::endl;

    // 只打印部分数据点，避免输出过多
    size_t step = std::max(1UL, equity.size() / 10);
    for (size_t i = 0; i < equity.size(); i += step)
    {
        std::cout << "Day " << i << ": " << std::fixed << std::setprecision(2) << equity[i] << " 元" << std::endl;
    }

    if (equity.size() > 0)
    {
        std::cout << "Final: " << std::fixed << std::setprecision(2) << equity.back() << " 元" << std::endl;
    }
}

// 将权益曲线保存到文件
void save_equity_curve(const std::vector<double> &equity, const std::string &filename)
{
    std::ofstream file(filename);
    if (file.is_open())
    {
        file << "Date,Equity" << std::endl;
        for (size_t i = 0; i < equity.size(); ++i)
        {
            file << i << "," << equity[i] << std::endl;
        }
        file.close();
        std::cout << "\n权益曲线已保存到文件: " << filename << std::endl;
    }
}

int main()
{
    std::cout << "欢迎使用股票量化回测系统！" << std::endl;
    std::cout << "===============================" << std::endl;

    // 创建数据处理器
    DataHandler data_handler;

    // 加载数据文件 - 使用真实A股数据
    std::string csv_file = "a_stock_data.csv"; // 请将真实数据保存为此文件名
    std::string symbol = "A_STOCK";

    std::ifstream check_file(csv_file);
    if (!check_file.good())
    {
        std::cout << "==========================================" << std::endl;
        std::cout << "警告：数据文件 " << csv_file << " 不存在！" << std::endl;
        std::cout << "请按以下步骤获取真实A股数据：" << std::endl;
        std::cout << "1. 从新浪财经、东方财富等网站下载股票历史数据" << std::endl;
        std::cout << "2. 保存为CSV格式，包含以下字段：日期,开盘价,最高价,最低价,收盘价,成交量" << std::endl;
        std::cout << "3. 将文件重命名为: " << csv_file << std::endl;
        std::cout << "4. 确保文件编码为UTF-8或GBK" << std::endl;
        std::cout << "==========================================" << std::endl;
        return 1;
    }

    // 加载数据
    std::cout << "\n正在加载数据文件: " << csv_file << std::endl;
    data_handler.load_data(csv_file, symbol);
    std::cout << "数据加载完成！" << std::endl;

    // 获取数据时间范围
    auto time_range = data_handler.get_time_range(symbol);
    auto start_time = time_range.first;
    auto end_time = time_range.second;

    // 创建策略（5日均线和10日均线交叉策略）
    MovingAverageCrossStrategy strategy(5, 10);

    // 创建执行处理器（初始资金100000元）
    ExecutionHandler executor(100000.0);

    // 创建回测引擎
    BacktestEngine engine(&data_handler, &strategy, &executor);

    // 运行回测
    std::cout << "\n正在运行回测..." << std::endl;
    engine.run(symbol, start_time, end_time);
    std::cout << "回测完成！" << std::endl;

    // 获取并打印回测结果
    const auto &result = engine.get_result();
    print_backtest_result(result);

    // 打印权益曲线
    print_equity_curve(engine.get_equity_curve());

    // 保存权益曲线到文件
    save_equity_curve(engine.get_equity_curve(), "equity_curve.csv");

    std::cout << "\n回测系统运行完毕！" << std::endl;

    return 0;
}
