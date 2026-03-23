#include "execution_handler.h"

ExecutionHandler::ExecutionHandler(double initial_cash)
{
    portfolio_.cash = initial_cash;
}

void ExecutionHandler::execute_signals(const std::vector<Signal> &signals, const std::vector<Bar> &bars)
{
    for (const auto &signal : signals)
    {
        // 查找对应的Bar数据
        double price = signal.price;
        std::string symbol = signal.symbol;

        if (signal.type == SignalType::BUY)
        {
            // 买入操作
            double quantity;

            if (signal.quantity > 0)
            {
                // 使用信号指定的数量
                quantity = signal.quantity;
            }
            else
            {
                // 使用默认策略：用全部现金购买
                quantity = portfolio_.cash / price;
            }

            quantity = std::floor(quantity); // 只购买整数股

            if (quantity > 0)
            {
                double cost = quantity * price;

                // 检查现金是否足够
                if (cost <= portfolio_.cash)
                {
                    // 更新现金
                    portfolio_.cash -= cost;

                    // 更新持仓
                    auto it = std::find_if(portfolio_.positions.begin(), portfolio_.positions.end(),
                                           [&symbol](const Position &pos)
                                           { return pos.symbol == symbol; });

                    if (it != portfolio_.positions.end())
                    {
                        // 更新现有持仓
                        double total_cost = it->quantity * it->average_price + cost;
                        it->quantity += quantity;
                        it->average_price = total_cost / it->quantity;
                    }
                    else
                    {
                        // 添加新持仓
                        Position new_position;
                        new_position.symbol = symbol;
                        new_position.quantity = quantity;
                        new_position.average_price = price;
                        portfolio_.positions.push_back(new_position);
                    }
                }
            }
        }
        else if (signal.type == SignalType::SELL)
        {
            // 卖出操作
            auto it = std::find_if(portfolio_.positions.begin(), portfolio_.positions.end(),
                                   [&symbol](const Position &pos)
                                   { return pos.symbol == symbol; });

            if (it != portfolio_.positions.end())
            {
                double quantity;

                if (signal.quantity > 0)
                {
                    // 使用信号指定的数量
                    quantity = std::min(signal.quantity, it->quantity);
                }
                else
                {
                    // 使用默认策略：卖出全部持仓
                    quantity = it->quantity;
                }

                quantity = std::floor(quantity); // 只卖出整数股

                if (quantity > 0)
                {
                    double proceeds = quantity * price;

                    // 更新现金
                    portfolio_.cash += proceeds;

                    // 更新或删除持仓
                    if (quantity == it->quantity)
                    {
                        // 全部卖出，删除持仓
                        portfolio_.positions.erase(it);
                    }
                    else
                    {
                        // 部分卖出，更新持仓
                        it->quantity -= quantity;
                    }
                }
            }
        }
    }
}

const Portfolio &ExecutionHandler::get_portfolio() const
{
    return portfolio_;
}

double ExecutionHandler::get_total_value() const
{
    return portfolio_.get_total_value();
}

double ExecutionHandler::get_cash() const
{
    return portfolio_.cash;
}

const std::vector<Position> &ExecutionHandler::get_positions() const
{
    return portfolio_.positions;
}

Position ExecutionHandler::find_position(const std::string &symbol) const
{
    auto it = std::find_if(portfolio_.positions.begin(), portfolio_.positions.end(),
                           [&symbol](const Position &pos)
                           { return pos.symbol == symbol; });

    if (it != portfolio_.positions.end())
    {
        return *it;
    }

    // 返回空持仓
    Position empty_position;
    empty_position.symbol = symbol;
    empty_position.quantity = 0;
    empty_position.average_price = 0;
    return empty_position;
}
