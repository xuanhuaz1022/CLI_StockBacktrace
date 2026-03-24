#include "strategy.h"
#include "indicator.h"
#include <sstream>

// MovingAverageCrossStrategy 构造函数
MovingAverageCrossStrategy::MovingAverageCrossStrategy(size_t short_period, size_t long_period)
    : short_period_(short_period), long_period_(long_period)
{
    std::stringstream ss;
    ss << "MA_Cross_" << short_period << "_" << long_period;
    name_ = ss.str();
}

// 初始化策略
void MovingAverageCrossStrategy::initialize()
{
    // 策略初始化代码（如果需要）
}

// 修改 strategy.cpp 中的 generate_signals 函数
std::vector<Signal> MovingAverageCrossStrategy::generate_signals(const std::vector<Bar> &bars, const std::string &symbol)
{
    std::vector<Signal> signals;

    if (bars.size() < 2)
    {
        return signals;
    }

    // 计算移动平均线
    std::vector<double> short_ma = simple_moving_average(bars, short_period_);
    std::vector<double> long_ma = simple_moving_average(bars, long_period_);

    // 只检查最后一个数据点的信号
    size_t i = bars.size() - 1;

    // 跳过数据不足的情况
    if (short_ma[i] == 0.0 || long_ma[i] == 0.0 ||
        short_ma[i - 1] == 0.0 || long_ma[i - 1] == 0.0)
    {
        return signals;
    }

    // 检测金叉（短期均线上穿长期均线）- 买入信号
    if (short_ma[i - 1] <= long_ma[i - 1] && short_ma[i] > long_ma[i])
    {
        Signal signal;
        signal.type = SignalType::BUY;
        signal.symbol = symbol;
        signal.price = bars[i].closePrice;
        signal.quantity = 0.0;
        signal.timestamp = bars[i].timestamp;
        signals.push_back(signal);
    }

    // 检测死叉（短期均线下穿长期均线）- 卖出信号
    if (short_ma[i - 1] >= long_ma[i - 1] && short_ma[i] < long_ma[i])
    {
        Signal signal;
        signal.type = SignalType::SELL;
        signal.symbol = symbol;
        signal.price = bars[i].closePrice;
        signal.quantity = 0.0;
        signal.timestamp = bars[i].timestamp;
        signals.push_back(signal);
    }

    return signals;
}

// 获取策略名称
std::string MovingAverageCrossStrategy::get_name() const
{
    return name_;
}
