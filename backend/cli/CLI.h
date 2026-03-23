#ifndef CLI_H
#define CLI_H

#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>
#include "../models/Database.h"
#include "../../core/include/backtest_engine.h"

using json = nlohmann::json;

namespace stock_backtrack
{

    class CLI
    {
    public:
        CLI();
        ~CLI();

        int run(int argc, char **argv);

    private:
        ::CLI::App app;
        Database db;
        BacktestEngine *engine_;

        void setup_data_commands();
        void setup_strategy_commands();
        void setup_backtest_commands();
        void setup_system_commands();

        // 数据管理命令处理
        void handle_data_upload(const std::string &file, const std::string &symbol, const std::string &format);
        void handle_data_list();
        void handle_data_show(const std::string &id);
        void handle_data_delete(const std::string &id);

        // 策略管理命令处理
        void handle_strategy_create(const std::string &name, const std::string &params);
        void handle_strategy_list();
        void handle_strategy_show(const std::string &id);
        void handle_strategy_update(const std::string &id, const std::string &name, const std::string &params);
        void handle_strategy_delete(const std::string &id);

        // 回测管理命令处理
        void handle_backtest_run(const std::string &data_id, const std::string &strategy_id, double equity);
        void handle_backtest_list();
        void handle_backtest_show(const std::string &id);
        void handle_backtest_equity(const std::string &id);
        void handle_backtest_status(const std::string &id);
        void handle_backtest_batch(const std::string &file);

        // 系统管理命令处理
        void handle_system_status();
        void handle_system_metrics();

        // 辅助函数
        std::string generate_id(const std::string &prefix);
        void print_json(const json &data);
        void print_error(const std::string &message);
        bool validate_file_path(const std::string &path);
        void handle_backtest_error(const std::string &backtest_id, const std::string &error_message);
    };

} // namespace stock_backtrack

#endif
