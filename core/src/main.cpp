#include "../backend/cli/CLI.h"
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

// 运行默认回测（兼容旧版功能）
void run_default_backtest()
{
    std::cout << "欢迎使用股票量化回测系统！" << std::endl;
    std::cout << "===============================" << std::endl;

    // 使用BaoStock格式的数据文件
    std::string csv_file = "data/sh_000001.csv"; // 使用上证指数数据
    std::string symbol = "上证指数";
    std::string data_format = "baostock";

    std::ifstream check_file(csv_file);
    if (!check_file.good())
    {
        std::cout << "==========================================" << std::endl;
        std::cout << "警告：数据文件 " << csv_file << " 不存在！" << std::endl;
        std::cout << "请确保已下载BaoStock数据到data文件夹" << std::endl;
        std::cout << "使用命令: python3 download_data.py" << std::endl;
        std::cout << "==========================================" << std::endl;
        return;
    }

    // 根据数据格式创建不同的DataHandler
    std::unique_ptr<DataHandler> data_handler;
    if (data_format == "baostock")
    {
        data_handler = std::make_unique<BaoStockDataHandler>();
    }
    else
    {
        data_handler = std::make_unique<DataHandler>();
    }

    // 加载数据
    std::cout << "\n正在加载数据文件: " << csv_file << std::endl;
    std::cout << "数据格式: " << data_format << std::endl;
    data_handler->load_data(csv_file, symbol);
    std::cout << "数据加载完成！" << std::endl;

    // 获取数据时间范围
    auto time_range = data_handler->get_time_range(symbol);
    auto start_time = time_range.first;
    auto end_time = time_range.second;

    // 创建策略（5日均线和10日均线交叉策略）
    MovingAverageCrossStrategy strategy(5, 10);

    // 创建执行处理器（初始资金100000元）
    ExecutionHandler executor(100000.0);

    // 创建回测引擎
    BacktestEngine engine(data_handler.get(), &strategy, &executor);

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
}

int main(int argc, char **argv)
{
    // 如果没有命令行参数，运行默认回测
    if (argc == 1)
    {
        run_default_backtest();
        return 0;
    }

    // 否则运行CLI接口
    try
    {
        stock_backtrack::CLI cli;
        return cli.run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}
