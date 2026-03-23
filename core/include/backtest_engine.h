#ifndef BACKTEST_ENGINE_H
#define BACKTEST_ENGINE_H

#include "types.h"
#include "strategy.h"
#include "execution_handler.h"
#include "data_handler.h"
#include <string>
#include <vector>
#include <chrono>

class BacktestEngine {
private:
    DataHandler* data_handler_;
    Strategy* strategy_;
    ExecutionHandler* executor_;
    BacktestResult result_;
    std::vector<double> equity_curve_;
    
public:
    BacktestEngine(DataHandler* data_handler, Strategy* strategy, ExecutionHandler* executor);
    
    // 运行回测
    void run(const std::string& symbol, 
             const std::chrono::system_clock::time_point& start_time,
             const std::chrono::system_clock::time_point& end_time);
    
    // 获取回测结果
    const BacktestResult& get_result() const;
    
    // 获取权益曲线
    const std::vector<double>& get_equity_curve() const;
    
private:
    // 计算绩效指标
    void calculate_metrics();
    
    // 计算最大回撤
    double calculate_max_drawdown(const std::vector<double>& equity) const;
    
    // 计算夏普比率（简化版本）
    double calculate_sharpe_ratio(const std::vector<double>& equity) const;
    
    // 计算日收益率序列
    std::vector<double> calculate_daily_returns(const std::vector<double>& equity) const;
};

#endif