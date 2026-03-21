#include <string>
#include <chrono>
#include <unordered_map>
#include <vector>
#include <iostream>

struct Bar
{
  std::chrono::system_clock::time_point timestamp;
  double openPrice;
  double highPrice;
  double lowPrice;
  double closePrice;
  double volume;
};

struct Order
{
  std::string order_id;
  std::string symbol;
  enum class Type
  {
    BUY,
    SELL
  };
  Type type;
  double price;
  double quantity;
  enum class Status
  {
    PENDING,
    FILLED,
    CANCELLED
  };
  Status status;
  std::chrono::system_clock::time_point timestamp;
};

struct Position
{
  std::string symbol;
  double quantity;
  double average_price;
};

struct Portfolio
{
  double cash;
  std::vector<Position> positions;

  double get_total_value() const;
};

struct BacktestResult
{
  std::string strategy_name;
  double initial_equity;
  double final_equity;
  double return_rate;
  double max_drawdown;
  double sharpe_ratio;
  double sortino_ratio;
  std::vector<double> equity_curve;
};