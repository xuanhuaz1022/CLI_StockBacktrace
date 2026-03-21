#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <chrono>
#include "types.h"
class DataHandler
{
public:
  DataHandler() = default;
  virtual ~DataHandler() = default;
  virtual void load_data(const std::string &file_path, const std::string &symbol);
  virtual std::vector<Bar> get_bars(const std::string &symbol,
                                    const std::chrono::system_clock::time_point &start_time,
                                    const std::chrono::system_clock::time_point &end_time);
  virtual std::vector<std::string> get_symbols();
  virtual std::pair<std::chrono::system_clock::time_point,
                    std::chrono::system_clock::time_point>
  get_time_range(const std::string &symbol);
  virtual bool has_data(const std::string &symbol);

private:
  std::unordered_map<std::string, std::vector<Bar>> data_;
};