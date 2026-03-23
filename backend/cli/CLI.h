#ifndef CLI_H
#define CLI_H

#include <CLI/CLI.hpp>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class CLI {
public:
    CLI();
    ~CLI();
    
    int run(int argc, char** argv);
    
private:
    CLI::App app_;
    
    // 数据管理命令
    void setup_data_commands();
    void handle_data_upload(const std::string& file, const std::string& symbol);
    void handle_data_list();
    void handle_data_show(const std::string& id);
    void handle_data_delete(const std::string& id);
    
    // 策略管理命令
    void setup_strategy_commands();
    void handle_strategy_create(const std::string& name, const std::string& params);
    void handle_strategy_list();
    void handle_strategy_show(const std::string& id);
    void handle_strategy_update(const std::string& id, const std::string& params);
    void handle_strategy_delete(const std::string& id);
    
    // 回测执行命令
    void setup_backtest_commands();
    void handle_backtest_run(const std::string& data_id, const std::string& strategy_id, 
                           double initial_equity);
    void handle_backtest_list();
    void handle_backtest_show(const std::string& id);
    void handle_backtest_equity(const std::string& id);
    void handle_backtest_status(const std::string& id);
    
    // 系统管理命令
    void setup_system_commands();
    void handle_system_status();
    void handle_system_metrics();
    
    // 工具方法
    void print_json(const json& data);
    void print_error(const std::string& message);
    std::string generate_id(const std::string& prefix);
};

#endif