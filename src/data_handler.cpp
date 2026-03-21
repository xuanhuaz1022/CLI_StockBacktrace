#include "data_handler.h"

void DataHandler::load_data(const std::string &file_path, const std::string &symbol)
{
  // 实现数据加载逻辑
  // 1. 创建对象并打开文件
  std::ifstream file(file_path);
  if (!file.is_open())
  {
    std::cerr << "Error: Failed to open file " << file_path << std::endl;
    return;
  }

  // 2. 读取文件内容
  std::string line;
  bool is_header = true; // 跳过表头行
  data_[symbol].reserve(1000000);
  while (std::getline(file, line))
  {
    if (is_header)
    {
      is_header = false;
      continue; // 跳过表头行
    }
    try
    {
      // 解析行数据
      std::stringstream ss(line);
      std::string temp;
      std::chrono::system_clock::time_point timestamp;
      double open, high, low, close, volume;

      // 解析时间戳
      std::getline(ss, temp, ',');
      std::tm tm = {};
      std::istringstream timestamp_ss(temp);
      timestamp_ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
      if (timestamp_ss.fail())
      {
        std::cerr << "Error parsing timestamp: " << temp << std::endl;
        continue;
      }
      auto time_t = std::mktime(&tm);
      timestamp = std::chrono::system_clock::from_time_t(time_t);

      // 解析其他字段
      std::getline(ss, temp, ',');
      open = std::stod(temp);
      std::getline(ss, temp, ',');
      high = std::stod(temp);
      std::getline(ss, temp, ',');
      low = std::stod(temp);
      std::getline(ss, temp, ',');
      close = std::stod(temp);
      std::getline(ss, temp, ',');
      volume = std::stod(temp);
      // 3. 存储数据
      data_[symbol].push_back({timestamp, open, high, low, close, volume});
    }
    catch (const std::exception &e)
    {
      std::cerr << "Error: " << e.what() << std::endl;
      continue;
    }
  }

  // 4. 关闭文件
  file.close();
}

std::vector<Bar> DataHandler::get_bars(const std::string &symbol,
                                       const std::chrono::system_clock::time_point &start_time,
                                       const std::chrono::system_clock::time_point &end_time)
{
  // 实现获取指定时间范围内的Bar数据逻辑
  if (!has_data(symbol))
  {
    return {};
  }
  const auto &bars = data_[symbol];
  std::vector<Bar> result;

  auto start_it = std::lower_bound(bars.begin(), bars.end(), start_time,
                                   [](const Bar &bar, const auto &time)
                                   {
                                     return bar.timestamp < time;
                                   });
  auto end_it = std::upper_bound(bars.begin(), bars.end(), end_time,
                                 [](const auto &time, const Bar &bar)
                                 {
                                   return time < bar.timestamp;
                                 });
  result.assign(start_it, end_it);
  return result;
}

std::vector<std::string> DataHandler::get_symbols()
{
  // 实现获取所有已加载股票符号的逻辑
  std::vector<std::string> result;
  for (const auto &[symbol, bars] : data_)
  {
    result.push_back(symbol);
  }
  return result;
}

std::pair<std::chrono::system_clock::time_point,
          std::chrono::system_clock::time_point>
DataHandler::get_time_range(const std::string &symbol)
{
  // 实现获取指定股票符号的时间范围逻辑
  if (data_.find(symbol) == data_.end())
  {
    return {};
  }
  return {data_[symbol].front().timestamp, data_[symbol].back().timestamp};
}

bool DataHandler::has_data(const std::string &symbol)
{
  // 实现检查指定股票符号是否有数据的逻辑
  return data_.find(symbol) != data_.end();
}
