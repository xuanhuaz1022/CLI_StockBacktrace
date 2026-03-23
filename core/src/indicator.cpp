#include "indicator.h"

std::vector<double> simple_moving_average(const std::vector<Bar> &bars, size_t period)
{
  std::vector<double> result(bars.size(), 0.0);

  if (bars.empty() || period == 0)
  {
    return result;
  }

  double sum = 0.0;
  size_t count = 0;

  for (size_t i = 0; i < bars.size(); ++i)
  {
    sum += bars[i].closePrice;
    count++;

    if (count > period)
    {
      sum -= bars[i - period].closePrice;
      count = period;
    }

    if (count == period)
    {
      result[i] = sum / period;
    }
  }

  return result;
}

std::vector<double> calculate_ema(const std::vector<Bar> &bars, size_t period)
{
  std::vector<double> result(bars.size(), 0.0);

  if (bars.empty() || period == 0)
  {
    return result;
  }

  // 计算平滑因子 alpha = 2 / (period + 1)
  double alpha = 2.0 / (period + 1);

  // 第一个 EMA 值使用 SMA 作为初始值
  double ema = 0.0;
  for (size_t i = 0; i < period; ++i)
  {
    ema += bars[i].closePrice;
  }
  ema /= period;
  result[period - 1] = ema;

  // 计算后续的 EMA 值
  for (size_t i = period; i < bars.size(); ++i)
  {
    ema = alpha * bars[i].closePrice + (1 - alpha) * ema;
    result[i] = ema;
  }

  return result;
}