#include "backtest_engine.h"
#include <cmath>
#include <iostream>

BacktestEngine::BacktestEngine(DataHandler* data_handler, Strategy* strategy, ExecutionHandler* executor)
    : data_handler_(data_handler), strategy_(strategy), executor_(executor) {
    result_.strategy_name = strategy_->get_name();
    result_.initial_equity = executor_->get_total_value();
}

void BacktestEngine::run(const std::string& symbol, 
                       const std::chrono::system_clock::time_point& start_time,
                       const std::chrono::system_clock::time_point& end_time) {
    // 获取指定时间范围内的数据
    std::vector<Bar> bars = data_handler_->get_bars(symbol, start_time, end_time);
    
    if (bars.empty()) {
        std::cout << "No data available for the specified period." << std::endl;
        return;
    }
    
    // 初始化策略
    strategy_->initialize();
    
    // 初始化权益曲线
    equity_curve_.clear();
    equity_curve_.push_back(executor_->get_total_value());
    
    // 按时间顺序处理每个数据点
    for (size_t i = 0; i < bars.size(); ++i) {
        // 获取当前时间点之前的所有数据
        std::vector<Bar> historical_data(bars.begin(), bars.begin() + i + 1);
        
        // 生成交易信号
        std::vector<Signal> signals = strategy_->generate_signals(historical_data);
        
        // 执行交易信号
        executor_->execute_signals(signals, historical_data);
        
        // 更新权益曲线
        equity_curve_.push_back(executor_->get_total_value());
    }
    
    // 计算绩效指标
    calculate_metrics();
}

const BacktestResult& BacktestEngine::get_result() const {
    return result_;
}

const std::vector<double>& BacktestEngine::get_equity_curve() const {
    return equity_curve_;
}

void BacktestEngine::calculate_metrics() {
    if (equity_curve_.empty()) {
        return;
    }
    
    // 计算最终权益
    result_.final_equity = equity_curve_.back();
    
    // 计算总收益率
    result_.return_rate = (result_.final_equity - result_.initial_equity) / result_.initial_equity;
    
    // 计算最大回撤
    result_.max_drawdown = calculate_max_drawdown(equity_curve_);
    
    // 计算夏普比率
    result_.sharpe_ratio = calculate_sharpe_ratio(equity_curve_);
    
    // 设置权益曲线
    result_.equity_curve = equity_curve_;
}

double BacktestEngine::calculate_max_drawdown(const std::vector<double>& equity) const {
    if (equity.size() <= 1) {
        return 0.0;
    }
    
    double max_drawdown = 0.0;
    double peak = equity[0];
    
    for (size_t i = 1; i < equity.size(); ++i) {
        if (equity[i] > peak) {
            peak = equity[i];
        }
        
        double drawdown = (peak - equity[i]) / peak;
        if (drawdown > max_drawdown) {
            max_drawdown = drawdown;
        }
    }
    
    return max_drawdown;
}

double BacktestEngine::calculate_sharpe_ratio(const std::vector<double>& equity) const {
    if (equity.size() <= 1) {
        return 0.0;
    }
    
    std::vector<double> returns = calculate_daily_returns(equity);
    
    if (returns.empty()) {
        return 0.0;
    }
    
    // 计算平均收益率
    double mean_return = 0.0;
    for (double r : returns) {
        mean_return += r;
    }
    mean_return /= returns.size();
    
    // 计算收益率的标准差
    double variance = 0.0;
    for (double r : returns) {
        variance += (r - mean_return) * (r - mean_return);
    }
    variance /= returns.size();
    double std_dev = std::sqrt(variance);
    
    if (std_dev == 0.0) {
        return 0.0;
    }
    
    // 简化版夏普比率：假设无风险利率为0
    // 实际应该使用年化收益率和年化标准差
    return mean_return / std_dev;
}

std::vector<double> BacktestEngine::calculate_daily_returns(const std::vector<double>& equity) const {
    std::vector<double> returns;
    
    for (size_t i = 1; i < equity.size(); ++i) {
        double daily_return = (equity[i] - equity[i-1]) / equity[i-1];
        returns.push_back(daily_return);
    }
    
    return returns;
}
