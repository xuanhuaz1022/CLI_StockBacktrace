#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace Utils {
    // 生成唯一ID
    std::string generate_id(const std::string& prefix);
    
    // 时间处理
    std::string format_time(const std::chrono::system_clock::time_point& time_point);
    std::chrono::system_clock::time_point parse_time(const std::string& time_str);
    
    // 文件处理
    bool file_exists(const std::string& file_path);
    std::string get_file_size(const std::string& file_path);
    std::vector<std::string> read_file_lines(const std::string& file_path);
    
    // JSON处理
    bool parse_json(const std::string& json_str, json& result);
    std::string format_json(const json& data);
    
    // 字符串处理
    std::string to_lower(const std::string& str);
    std::string trim(const std::string& str);
    std::vector<std::string> split(const std::string& str, char delimiter);
    
    // 数值计算
    double calculate_return_rate(double initial, double final);
    double calculate_max_drawdown(const std::vector<double>& values);
    double calculate_sharpe_ratio(const std::vector<double>& returns, double risk_free_rate = 0.02);
    
    // 验证函数
    bool validate_symbol(const std::string& symbol);
    bool validate_price(double price);
    bool validate_quantity(double quantity);
}

#endif