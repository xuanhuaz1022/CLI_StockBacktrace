#ifndef STRATEGY_H
#define STRATEGY_H

#include "types.h"
#include <string>
#include <vector>

// 信号类型枚举
enum class SignalType {
    BUY,
    SELL,
    HOLD
};

// 信号结构体
struct Signal {
    SignalType type;
    std::string symbol;
    double price;
    double quantity;  // 交易数量，0表示使用默认数量策略
    std::chrono::system_clock::time_point timestamp;
};

// 策略基类
class Strategy {
public:
    virtual ~Strategy() = default;
    
    // 初始化策略
    virtual void initialize() = 0;
    
    // 处理数据并生成信号
    virtual std::vector<Signal> generate_signals(const std::vector<Bar>& bars, const std::string& symbol) = 0;
    
    // 获取策略名称
    virtual std::string get_name() const = 0;
};

// 移动平均线交叉策略
class MovingAverageCrossStrategy : public Strategy {
private:
    size_t short_period_;
    size_t long_period_;
    std::string name_;
    
public:
    MovingAverageCrossStrategy(size_t short_period, size_t long_period);
    
    void initialize() override;
    std::vector<Signal> generate_signals(const std::vector<Bar>& bars, const std::string& symbol) override;
    std::string get_name() const override;
};

#endif