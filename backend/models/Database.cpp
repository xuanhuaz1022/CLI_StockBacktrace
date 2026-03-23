#include "Database.h"
#include <iostream>
#include <cstring>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <string>

Database::Database() : db_(nullptr) {}

Database::~Database()
{
    disconnect();
}

bool Database::connect(const std::string &db_path)
{
    std::cout << "正在连接数据库: " << db_path << std::endl;
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK)
    {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }

    std::cout << "数据库连接成功" << std::endl;

    if (!create_tables())
    {
        std::cerr << "创建表失败" << std::endl;
        disconnect();
        return false;
    }

    std::cout << "成功连接到数据库: " << db_path << std::endl;
    return true;
}

bool Database::disconnect()
{
    if (db_)
    {
        sqlite3_close(db_);
        db_ = nullptr;
        std::cout << "数据库连接已关闭" << std::endl;
        return true;
    }
    return false;
}

bool Database::execute_query(const std::string &query)
{
    char *err_msg;
    int rc = sqlite3_exec(db_, query.c_str(), nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL执行错误: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool Database::execute_query_with_callback(const std::string &query,
                                           int (*callback)(void *, int, char **, char **),
                                           void *data)
{
    char *err_msg;
    int rc = sqlite3_exec(db_, query.c_str(), callback, data, &err_msg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL执行错误: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }
    return true;
}

bool Database::execute_prepared_statement(const std::string &query,
                                          const std::vector<std::pair<int, std::string>> &parameters)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // 绑定参数
    for (const auto &param : parameters)
    {
        int index = param.first;
        const std::string &value = param.second;

        // 使用更可靠的类型判断方法
        if (value.empty())
        {
            rc = sqlite3_bind_null(stmt, index);
        }
        else
        {
            try
            {
                // 尝试转换为整数
                size_t pos;
                long long int_val = std::stoll(value, &pos);
                if (pos == value.size())
                {
                    // 完全匹配整数格式
                    rc = sqlite3_bind_int64(stmt, index, int_val);
                }
                else
                {
                    // 尝试转换为浮点数
                    double double_val = std::stod(value);
                    rc = sqlite3_bind_double(stmt, index, double_val);
                }
            }
            catch (const std::invalid_argument &)
            {
                // 不是数字，绑定为文本
                rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
            }
            catch (const std::out_of_range &)
            {
                // 数字超出范围，绑定为文本
                rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
            }
        }

        if (rc != SQLITE_OK)
        {
            std::cerr << "绑定参数失败: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "执行SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::execute_prepared_statement_with_callback(const std::string &query,
                                                        const std::vector<std::pair<int, std::string>> &parameters,
                                                        int (*callback)(void *, int, char **, char **),
                                                        void *data)
{
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "准备SQL语句失败: " << sqlite3_errmsg(db_) << std::endl;
        return false;
    }

    // 绑定参数
    for (const auto &param : parameters)
    {
        int index = param.first;
        const std::string &value = param.second;

        // 使用更可靠的类型判断方法
        if (value.empty())
        {
            rc = sqlite3_bind_null(stmt, index);
        }
        else
        {
            try
            {
                // 尝试转换为整数
                size_t pos;
                long long int_val = std::stoll(value, &pos);
                if (pos == value.size())
                {
                    // 完全匹配整数格式
                    rc = sqlite3_bind_int64(stmt, index, int_val);
                }
                else
                {
                    // 尝试转换为浮点数
                    double double_val = std::stod(value);
                    rc = sqlite3_bind_double(stmt, index, double_val);
                }
            }
            catch (const std::invalid_argument &)
            {
                // 不是数字，绑定为文本
                rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
            }
            catch (const std::out_of_range &)
            {
                // 数字超出范围，绑定为文本
                rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
            }
        }

        if (rc != SQLITE_OK)
        {
            std::cerr << "绑定参数失败: " << sqlite3_errmsg(db_) << std::endl;
            sqlite3_finalize(stmt);
            return false;
        }
    }

    // 执行查询并处理结果
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        int column_count = sqlite3_column_count(stmt);
        char **argv = new char *[column_count];
        char **azColName = new char *[column_count];

        for (int i = 0; i < column_count; ++i)
        {
            const char *column_name = sqlite3_column_name(stmt, i);
            azColName[i] = const_cast<char *>(column_name);

            const char *value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            if (value)
            {
                argv[i] = const_cast<char *>(value);
            }
            else
            {
                argv[i] = nullptr;
            }
        }

        callback(data, column_count, argv, azColName);

        delete[] argv;
        delete[] azColName;
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "执行SQL查询失败: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::create_tables()
{
    // 创建数据文件表
    std::string create_data_files = R"(
        CREATE TABLE IF NOT EXISTS data_files (
            id TEXT PRIMARY KEY,
            symbol TEXT NOT NULL,
            file_path TEXT NOT NULL,
            start_date DATETIME NOT NULL,
            end_date DATETIME NOT NULL,
            record_count INTEGER NOT NULL,
            format TEXT NOT NULL DEFAULT 'default',
            upload_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // 创建策略表
    std::string create_strategies = R"(
        CREATE TABLE IF NOT EXISTS strategies (
            id TEXT PRIMARY KEY,
            name TEXT NOT NULL,
            type TEXT NOT NULL,
            parameters TEXT NOT NULL,
            created_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
    )";

    // 创建回测表
    std::string create_backtests = R"(
        CREATE TABLE IF NOT EXISTS backtests (
            id TEXT PRIMARY KEY,
            data_id TEXT NOT NULL,
            strategy_id TEXT NOT NULL,
            initial_equity REAL NOT NULL,
            final_equity REAL,
            return_rate REAL,
            max_drawdown REAL,
            sharpe_ratio REAL,
            status TEXT NOT NULL,
            progress INTEGER NOT NULL DEFAULT 0,
            start_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
            end_time DATETIME,
            FOREIGN KEY (data_id) REFERENCES data_files(id),
            FOREIGN KEY (strategy_id) REFERENCES strategies(id)
        );
    )";

    // 创建权益曲线表
    std::string create_equity_curves = R"(
        CREATE TABLE IF NOT EXISTS equity_curves (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            backtest_id TEXT NOT NULL,
            timestamp DATETIME NOT NULL,
            equity REAL NOT NULL,
            FOREIGN KEY (backtest_id) REFERENCES backtests(id)
        );
    )";

    return execute_query(create_data_files) &&
           execute_query(create_strategies) &&
           execute_query(create_backtests) &&
           execute_query(create_equity_curves);
}

// 数据文件相关操作
bool Database::create_data_file(const std::string &id, const std::string &symbol,
                                const std::string &file_path, const std::string &start_date,
                                const std::string &end_date, int record_count,
                                const std::string &format)
{
    std::string query = "INSERT INTO data_files (id, symbol, file_path, start_date, end_date, record_count, format) "
                        "VALUES (?, ?, ?, ?, ?, ?, ?)";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id},
        {2, symbol},
        {3, file_path},
        {4, start_date},
        {5, end_date},
        {6, std::to_string(record_count)},
        {7, format}};

    return execute_prepared_statement(query, parameters);
}

bool Database::get_data_files(std::vector<json> &data_files)
{
    std::string query = "SELECT * FROM data_files ORDER BY upload_time DESC";

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &files = *static_cast<std::vector<json> *>(data);
        json file;
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                file[azColName[i]] = argv[i];
            }
            else
            {
                file[azColName[i]] = nullptr;
            }
        }
        files.push_back(file);
        return 0;
    };

    return execute_query_with_callback(query, callback, &data_files);
}

bool Database::get_data_file(const std::string &id, json &data_file)
{
    std::string query = "SELECT * FROM data_files WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id}};

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &file = *static_cast<json *>(data);
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                file[azColName[i]] = argv[i];
            }
            else
            {
                file[azColName[i]] = nullptr;
            }
        }
        return 0;
    };

    return execute_prepared_statement_with_callback(query, parameters, callback, &data_file);
}

bool Database::delete_data_file(const std::string &id)
{
    std::string query = "DELETE FROM data_files WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id}};

    return execute_prepared_statement(query, parameters);
}

// 策略相关操作
bool Database::create_strategy(const std::string &id, const std::string &name,
                               const std::string &type, const std::string &parameters)
{
    std::string query = "INSERT INTO strategies (id, name, type, parameters) "
                        "VALUES (?, ?, ?, ?)";

    std::vector<std::pair<int, std::string>> parameters_list = {
        {1, id},
        {2, name},
        {3, type},
        {4, parameters}};

    return execute_prepared_statement(query, parameters_list);
}

bool Database::get_strategies(std::vector<json> &strategies)
{
    std::string query = "SELECT * FROM strategies ORDER BY created_at DESC";

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &strats = *static_cast<std::vector<json> *>(data);
        json strat;
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                strat[azColName[i]] = argv[i];
            }
            else
            {
                strat[azColName[i]] = nullptr;
            }
        }
        strats.push_back(strat);
        return 0;
    };

    return execute_query_with_callback(query, callback, &strategies);
}

bool Database::get_strategy(const std::string &id, json &strategy)
{
    std::string query = "SELECT * FROM strategies WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id}};

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &strat = *static_cast<json *>(data);
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                strat[azColName[i]] = argv[i];
            }
            else
            {
                strat[azColName[i]] = nullptr;
            }
        }
        return 0;
    };

    return execute_prepared_statement_with_callback(query, parameters, callback, &strategy);
}

bool Database::update_strategy(const std::string &id, const std::string &name,
                               const std::string &parameters)
{
    std::string query = "UPDATE strategies SET name = ?, parameters = ?, updated_at = CURRENT_TIMESTAMP WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters_list = {
        {1, name},
        {2, parameters},
        {3, id}};

    return execute_prepared_statement(query, parameters_list);
}

bool Database::delete_strategy(const std::string &id)
{
    std::string query = "DELETE FROM strategies WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id}};

    return execute_prepared_statement(query, parameters);
}

// 回测相关操作
bool Database::create_backtest(const std::string &id, const std::string &data_id,
                               const std::string &strategy_id, double initial_equity)
{
    std::string query = "INSERT INTO backtests (id, data_id, strategy_id, initial_equity, status) "
                        "VALUES (?, ?, ?, ?, 'running')";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id},
        {2, data_id},
        {3, strategy_id},
        {4, std::to_string(initial_equity)}};

    return execute_prepared_statement(query, parameters);
}

bool Database::update_backtest_status(const std::string &id, const std::string &status,
                                      int progress, double final_equity,
                                      double return_rate, double max_drawdown,
                                      double sharpe_ratio)
{
    std::string query = "UPDATE backtests SET status = ?, progress = ?";
    std::vector<std::pair<int, std::string>> parameters = {
        {1, status},
        {2, std::to_string(progress)}};

    int param_index = 3;

    if (final_equity > 0)
    {
        query += ", final_equity = ?";
        parameters.emplace_back(param_index++, std::to_string(final_equity));
    }
    if (return_rate > 0)
    {
        query += ", return_rate = ?";
        parameters.emplace_back(param_index++, std::to_string(return_rate));
    }
    if (max_drawdown > 0)
    {
        query += ", max_drawdown = ?";
        parameters.emplace_back(param_index++, std::to_string(max_drawdown));
    }
    if (sharpe_ratio > 0)
    {
        query += ", sharpe_ratio = ?";
        parameters.emplace_back(param_index++, std::to_string(sharpe_ratio));
    }
    if (status == "completed" || status == "failed")
    {
        query += ", end_time = CURRENT_TIMESTAMP";
    }

    query += " WHERE id = ?";
    parameters.emplace_back(param_index, id);

    return execute_prepared_statement(query, parameters);
}

bool Database::get_backtests(std::vector<json> &backtests)
{
    std::string query = "SELECT * FROM backtests ORDER BY start_time DESC";

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &tests = *static_cast<std::vector<json> *>(data);
        json test;
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                // 尝试转换数值类型
                try
                {
                    double num = std::stod(argv[i]);
                    test[azColName[i]] = num;
                }
                catch (...)
                {
                    test[azColName[i]] = argv[i];
                }
            }
            else
            {
                test[azColName[i]] = nullptr;
            }
        }
        tests.push_back(test);
        return 0;
    };

    return execute_query_with_callback(query, callback, &backtests);
}

bool Database::get_backtest(const std::string &id, json &backtest)
{
    std::string query = "SELECT * FROM backtests WHERE id = ?";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, id}};

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &test = *static_cast<json *>(data);
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                // 尝试转换数值类型
                try
                {
                    double num = std::stod(argv[i]);
                    test[azColName[i]] = num;
                }
                catch (...)
                {
                    test[azColName[i]] = argv[i];
                }
            }
            else
            {
                test[azColName[i]] = nullptr;
            }
        }
        return 0;
    };

    return execute_prepared_statement_with_callback(query, parameters, callback, &backtest);
}

// 权益曲线相关操作
bool Database::add_equity_point(const std::string &backtest_id, const std::string &timestamp,
                                double equity)
{
    std::string query = "INSERT INTO equity_curves (backtest_id, timestamp, equity) "
                        "VALUES (?, ?, ?)";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, backtest_id},
        {2, timestamp},
        {3, std::to_string(equity)}};

    return execute_prepared_statement(query, parameters);
}

bool Database::get_equity_curve(const std::string &backtest_id, std::vector<json> &equity_curve)
{
    std::string query = "SELECT timestamp, equity FROM equity_curves WHERE backtest_id = ? ORDER BY timestamp";

    std::vector<std::pair<int, std::string>> parameters = {
        {1, backtest_id}};

    auto callback = [](void *data, int argc, char **argv, char **azColName) -> int
    {
        auto &curve = *static_cast<std::vector<json> *>(data);
        json point;
        for (int i = 0; i < argc; ++i)
        {
            if (argv[i])
            {
                if (std::string(azColName[i]) == "equity")
                {
                    point[azColName[i]] = std::stod(argv[i]);
                }
                else
                {
                    point[azColName[i]] = argv[i];
                }
            }
            else
            {
                point[azColName[i]] = nullptr;
            }
        }
        curve.push_back(point);
        return 0;
    };

    return execute_prepared_statement_with_callback(query, parameters, callback, &equity_curve);
}