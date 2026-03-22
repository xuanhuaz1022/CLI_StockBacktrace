#include "strategy.h"
#include "indicator.h"
#include <sstream>

// MovingAverageCrossStrategy 构造函数
MovingAverageCrossStrategy::MovingAverageCrossStrategy(size_t short_period, size_t long_period)
    : short_period_(short_period), long_period_(long_period) {
    std::stringstream ss;
    ss << "MA_Cross_" << short_period << "_" << long_period;
    name_ = ss.str();
}

// 初始化策略
void MovingAverageCrossStrategy::initialize() {
    // 策略初始化代码（如果需要）
}

// 生成信号的核心逻辑
std::vector<Signal> MovingAverageCrossStrategy::generate_signals(const std::vector<Bar>& bars) {
    std::vector<Signal> signals;
    
    if (bars.empty()) {
        return signals;
    }
    
    // 计算短期和长期移动平均线
    std::vector<double> short_ma = simple_moving_average(bars, short_period_);
    std::vector<double> long_ma = simple_moving_average(bars, long_period_);
    
    // 遍历数据，检测均线交叉
    for (size_t i = 1; i < bars.size(); ++i) {
        // 跳过数据不足的情况
        if (short_ma[i] == 0.0 || long_ma[i] == 0.0 || 
            short_ma[i-1] == 0.0 || long_ma[i-1] == 0.0) {
            continue;
        }
        
        // 检测金叉（短期均线上穿长期均线）- 买入信号
        if (short_ma[i-1] <= long_ma[i-1] && short_ma[i] > long_ma[i]) {
            Signal signal;
            signal.type = SignalType::BUY;
            signal.symbol = "Unknown";  // 实际应用中需要设置具体的股票代码
            signal.price = bars[i].closePrice;
            signal.quantity = 0.0;  // 0表示使用默认数量策略（全部资金买入）
            signal.timestamp = bars[i].timestamp;
            signals.push_back(signal);
        }
        
        // 检测死叉（短期均线下穿长期均线）- 卖出信号
        if (short_ma[i-1] >= long_ma[i-1] && short_ma[i] < long_ma[i]) {
            Signal signal;
            signal.type = SignalType::SELL;
            signal.symbol = "Unknown";  // 实际应用中需要设置具体的股票代码
            signal.price = bars[i].closePrice;
            signal.quantity = 0.0;  // 0表示使用默认数量策略（全部卖出）
            signal.timestamp = bars[i].timestamp;
            signals.push_back(signal);
        }
    }
    
    return signals;
}

// 获取策略名称
std::string MovingAverageCrossStrategy::get_name() const {
    return name_;
}
