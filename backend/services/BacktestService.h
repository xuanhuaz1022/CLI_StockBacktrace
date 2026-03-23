#ifndef BACKTEST_SERVICE_H
#define BACKTEST_SERVICE_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "../models/Database.h"

using json = nlohmann::json;

class BacktestService {
public:
    BacktestService(Database* database);
    ~BacktestService();
    
    // 运行回测
    std::string run_backtest(const std::string& data_id, const std::string& strategy_id, 
                           double initial_equity);
    
    // 获取回测状态
    json get_backtest_status(const std::string& backtest_id);
    
    // 获取回测结果
    json get_backtest_result(const std::string& backtest_id);
    
    // 获取权益曲线
    std::vector<json> get_equity_curve(const std::string& backtest_id);
    
    // 获取所有回测
    std::vector<json> get_all_backtests();
    
    // 取消回测
    bool cancel_backtest(const std::string& backtest_id);
    
private:
    Database* database_;
    
    // 执行回测逻辑
    void execute_backtest(const std::string& backtest_id, const std::string& data_id, 
                        const std::string& strategy_id, double initial_equity);
    
    // 更新回测进度
    void update_progress(const std::string& backtest_id, int progress);
    
    // 保存回测结果
    bool save_backtest_result(const std::string& backtest_id, double final_equity,
                            double return_rate, double max_drawdown, double sharpe_ratio);
    
    // 保存权益曲线
    bool save_equity_curve(const std::string& backtest_id, const std::vector<double>& equity_curve);
};

#endif