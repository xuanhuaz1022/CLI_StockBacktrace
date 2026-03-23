#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Database
{
public:
    Database();
    ~Database();

    bool connect(const std::string &db_path = "stock_backtrack.db");
    bool disconnect();

    // 数据文件相关操作
    bool create_data_file(const std::string &id, const std::string &symbol,
                          const std::string &file_path, const std::string &start_date,
                          const std::string &end_date, int record_count);
    bool get_data_files(std::vector<json> &data_files);
    bool get_data_file(const std::string &id, json &data_file);
    bool delete_data_file(const std::string &id);

    // 策略相关操作
    bool create_strategy(const std::string &id, const std::string &name,
                         const std::string &type, const std::string &parameters);
    bool get_strategies(std::vector<json> &strategies);
    bool get_strategy(const std::string &id, json &strategy);
    bool update_strategy(const std::string &id, const std::string &name,
                         const std::string &parameters);
    bool delete_strategy(const std::string &id);

    // 回测相关操作
    bool create_backtest(const std::string &id, const std::string &data_id,
                         const std::string &strategy_id, double initial_equity);
    bool update_backtest_status(const std::string &id, const std::string &status,
                                int progress, double final_equity = 0,
                                double return_rate = 0, double max_drawdown = 0,
                                double sharpe_ratio = 0);
    bool get_backtests(std::vector<json> &backtests);
    bool get_backtest(const std::string &id, json &backtest);

    // 权益曲线相关操作
    bool add_equity_point(const std::string &backtest_id, const std::string &timestamp,
                          double equity);
    bool get_equity_curve(const std::string &backtest_id, std::vector<json> &equity_curve);

private:
    sqlite3 *db_;

    bool execute_query(const std::string &query);
    bool execute_query_with_callback(const std::string &query,
                                     int (*callback)(void *, int, char **, char **),
                                     void *data);
    bool execute_prepared_statement(const std::string &query,
                                    const std::vector<std::pair<int, std::string>> &parameters);
    bool execute_prepared_statement_with_callback(const std::string &query,
                                                  const std::vector<std::pair<int, std::string>> &parameters,
                                                  int (*callback)(void *, int, char **, char **),
                                                  void *data);
    bool create_tables();
};

#endif