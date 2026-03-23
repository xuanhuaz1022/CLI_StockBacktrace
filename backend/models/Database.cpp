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
    int rc = sqlite3_open(db_path.c_str(), &db_);
    if (rc != SQLITE_OK)
    {
        std::cerr << "无法打开数据库: " << sqlite3_errmsg(db_) << std::endl;
        sqlite3_close(db_);
        db_ = nullptr;
        return false;
    }

    if (!create_tables())
    {
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
                                const std::string &end_date, int record_count)
{
    std::stringstream query;
    query << "INSERT INTO data_files (id, symbol, file_path, start_date, end_date, record_count) "
          << "VALUES ('" << id << "', '" << symbol << "', '" << file_path << "', '"
          << start_date << "', '" << end_date << "', " << record_count << ")";
    return execute_query(query.str());
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
    std::stringstream query;
    query << "SELECT * FROM data_files WHERE id = '" << id << "'";

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

    return execute_query_with_callback(query.str(), callback, &data_file);
}

bool Database::delete_data_file(const std::string &id)
{
    std::stringstream query;
    query << "DELETE FROM data_files WHERE id = '" << id << "'";
    return execute_query(query.str());
}

// 策略相关操作
bool Database::create_strategy(const std::string &id, const std::string &name,
                               const std::string &type, const std::string &parameters)
{
    std::string params_str = parameters;
    // 转义单引号
    size_t pos = 0;
    while ((pos = params_str.find("'", pos)) != std::string::npos)
    {
        params_str.replace(pos, 1, "''");
        pos += 2;
    }

    std::stringstream query;
    query << "INSERT INTO strategies (id, name, type, parameters) "
          << "VALUES ('" << id << "', '" << name << "', '" << type << "', '" << params_str << "')";
    return execute_query(query.str());
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
    std::stringstream query;
    query << "SELECT * FROM strategies WHERE id = '" << id << "'";

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

    return execute_query_with_callback(query.str(), callback, &strategy);
}

bool Database::update_strategy(const std::string &id, const std::string &name,
                               const std::string &parameters)
{
    std::string params_str = parameters;
    // 转义单引号
    size_t pos = 0;
    while ((pos = params_str.find("'", pos)) != std::string::npos)
    {
        params_str.replace(pos, 1, "''");
        pos += 2;
    }

    std::stringstream query;
    query << "UPDATE strategies SET name = '" << name << "', parameters = '" << params_str
          << "', updated_at = CURRENT_TIMESTAMP WHERE id = '" << id << "'";
    return execute_query(query.str());
}

bool Database::delete_strategy(const std::string &id)
{
    std::stringstream query;
    query << "DELETE FROM strategies WHERE id = '" << id << "'";
    return execute_query(query.str());
}

// 回测相关操作
bool Database::create_backtest(const std::string &id, const std::string &data_id,
                               const std::string &strategy_id, double initial_equity)
{
    std::stringstream query;
    query << "INSERT INTO backtests (id, data_id, strategy_id, initial_equity, status) "
          << "VALUES ('" << id << "', '" << data_id << "', '" << strategy_id << "', "
          << initial_equity << ", 'running')";
    return execute_query(query.str());
}

bool Database::update_backtest_status(const std::string &id, const std::string &status,
                                      int progress, double final_equity,
                                      double return_rate, double max_drawdown,
                                      double sharpe_ratio)
{
    std::stringstream query;
    query << "UPDATE backtests SET status = '" << status << "', progress = " << progress;

    if (final_equity > 0)
    {
        query << ", final_equity = " << final_equity;
    }
    if (return_rate > 0)
    {
        query << ", return_rate = " << return_rate;
    }
    if (max_drawdown > 0)
    {
        query << ", max_drawdown = " << max_drawdown;
    }
    if (sharpe_ratio > 0)
    {
        query << ", sharpe_ratio = " << sharpe_ratio;
    }
    if (status == "completed" || status == "failed")
    {
        query << ", end_time = CURRENT_TIMESTAMP";
    }

    query << " WHERE id = '" << id << "'";

    return execute_query(query.str());
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
    std::stringstream query;
    query << "SELECT * FROM backtests WHERE id = '" << id << "'";

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

    return execute_query_with_callback(query.str(), callback, &backtest);
}

// 权益曲线相关操作
bool Database::add_equity_point(const std::string &backtest_id, const std::string &timestamp,
                                double equity)
{
    std::stringstream query;
    query << "INSERT INTO equity_curves (backtest_id, timestamp, equity) "
          << "VALUES ('" << backtest_id << "', '" << timestamp << "', " << equity << ")";
    return execute_query(query.str());
}

bool Database::get_equity_curve(const std::string &backtest_id, std::vector<json> &equity_curve)
{
    std::stringstream query;
    query << "SELECT timestamp, equity FROM equity_curves WHERE backtest_id = '"
          << backtest_id << "' ORDER BY timestamp";

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

    return execute_query_with_callback(query.str(), callback, &equity_curve);
}