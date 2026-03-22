#include "../include/database.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

Database::Database(const std::string& host, const std::string& port, 
                  const std::string& dbname, const std::string& user, 
                  const std::string& password) 
    : connected_(false) {
    connection_string_ = "host=" + host + " port=" + port + 
                        " dbname=" + dbname + " user=" + user + 
                        " password=" + password;
}

Database::~Database() {
    disconnect();
}

bool Database::connect() {
    try {
        conn_ = std::make_unique<pqxx::connection>(connection_string_);
        if (conn_->is_open()) {
            connected_ = true;
            std::cout << "成功连接到数据库: " << conn_->dbname() << std::endl;
            return true;
        } else {
            std::cerr << "数据库连接失败" << std::endl;
            return false;
        }
    } catch (const pqxx::sql_error& e) {
        std::cerr << "SQL错误: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "连接错误: " << e.what() << std::endl;
        return false;
    }
}

void Database::disconnect() {
    if (connected_) {
        if (transaction_) {
            try {
                transaction_->abort();
            } catch (...) {}
            transaction_.reset();
        }
        conn_->close();
        connected_ = false;
        std::cout << "数据库连接已关闭" << std::endl;
    }
}

bool Database::is_connected() const {
    return connected_;
}

bool Database::insert_stock_basic(const std::string& symbol, const std::string& name,
                                const std::string& industry, const std::string& market,
                                const std::string& list_date) {
    if (!connected_) {
        std::cerr << "数据库未连接" << std::endl;
        return false;
    }
    
    try {
        pqxx::work txn(*conn_);
        std::string query = "INSERT INTO stock_basic (symbol, name, industry, market, list_date) "
                          "VALUES ($1, $2, $3, $4, $5) "
                          "ON CONFLICT (symbol) DO UPDATE "
                          "SET name = $2, industry = $3, market = $4, list_date = $5, updated_at = CURRENT_TIMESTAMP";
        
        txn.exec_params(query, symbol, name, industry, market, list_date);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入股票基本信息失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::get_stock_basic(const std::string& symbol, std::map<std::string, std::string>& info) {
    if (!connected_) {
        std::cerr << "数据库未连接" << std::endl;
        return false;
    }
    
    try {
        pqxx::work txn(*conn_);
        std::string query = "SELECT * FROM stock_basic WHERE symbol = $1";
        pqxx::result result = txn.exec_params(query, symbol);
        
        if (result.empty()) {
            return false;
        }
        
        const auto& row = result[0];
        info["symbol"] = row["symbol"].as<std::string>();
        info["name"] = row["name"].as<std::string>();
        info["industry"] = row["industry"].as<std::string>();
        info["market"] = row["market"].as<std::string>();
        info["list_date"] = row["list_date"].as<std::string>();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "查询股票基本信息失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::insert_stock_bar(const std::string& symbol, const Bar& bar) {
    if (!connected_) {
        std::cerr << "数据库未连接" << std::endl;
        return false;
    }
    
    try {
        // 将time_point转换为字符串
        std::time_t time = std::chrono::system_clock::to_time_t(bar.timestamp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        std::string timestamp_str = ss.str();
        
        pqxx::work txn(*conn_);
        std::string query = "INSERT INTO stock_bar (symbol, timestamp, open_price, high_price, "
                          "low_price, close_price, volume) "
                          "VALUES ($1, $2, $3, $4, $5, $6, $7) "
                          "ON CONFLICT (symbol, timestamp) DO UPDATE "
                          "SET open_price = $3, high_price = $4, low_price = $5, "
                          "close_price = $6, volume = $7";
        
        txn.exec_params(query, symbol, timestamp_str, bar.openPrice, bar.highPrice,
                       bar.lowPrice, bar.closePrice, bar.volume);
        txn.commit();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "插入K线数据失败: " << e.what() << std::endl;
        return false;
    }
}

bool Database::batch_insert_stock_bars(const std::string& symbol, const std::vector<Bar>& bars) {
    if (!connected_) {
        std::cerr << "数据库未连接" << std::endl;
        return false;
    }
    
    if (bars.empty()) {
        return true;
    }
    
    try {
        pqxx::work txn(*conn_);
        pqxx::stream_to stream(txn, "stock_bar");
        
        for (const auto& bar : bars) {
            std::time_t time = std::chrono::system_clock::to_time_t(bar.timestamp);
            std::stringstream ss;
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
            std::string timestamp_str = ss.str();
            
            stream << pqxx::row{
                pqxx::field{"symbol", symbol},
                pqxx::field{"timestamp", timestamp_str},
                pqxx::field{"open_price", bar.openPrice},
                pqxx::field{"high_price", bar.highPrice},
                pqxx::field{"low_price