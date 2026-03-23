#ifndef EXECUTION_HANDLER_H
#define EXECUTION_HANDLER_H

#include "types.h"
#include "strategy.h"
#include <vector>
#include <string>

class ExecutionHandler {
private:
    Portfolio portfolio_;
    
public:
    ExecutionHandler(double initial_cash);
    
    // 执行信号并更新投资组合
    void execute_signals(const std::vector<Signal>& signals, const std::vector<Bar>& bars);
    
    // 获取当前投资组合状态
    const Portfolio& get_portfolio() const;
    
    // 获取投资组合总价值
    double get_total_value() const;
    
    // 获取现金余额
    double get_cash() const;
    
    // 获取持仓信息
    const std::vector<Position>& get_positions() const;
    
    // 查找特定股票的持仓
    Position find_position(const std::string& symbol) const;
};

#endif