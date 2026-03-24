#ifndef API_SERVER_H
#define API_SERVER_H

#include <drogon/HttpAppFramework.h>
#include <string>
#include <nlohmann/json.hpp>

class APIServer {
public:
    APIServer();
    ~APIServer();
    
    bool start(int port = 8080);
    void stop();
    
private:
    drogon::HttpAppFramework& app_;
    
    // 路由配置
    void setup_routes();
    
    // 数据管理路由
    void setup_data_routes();
    void handle_data_upload(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_data_list(const drogon::HttpRequestPtr& req,
                         std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_data_detail(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_data_delete(const drogon::HttpRequestPtr& req,
                           std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    
    // 策略管理路由
    void setup_strategy_routes();
    void handle_strategy_create(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_strategy_list(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_strategy_detail(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_strategy_update(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_strategy_delete(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    
    // 回测管理路由
    void setup_backtest_routes();
    void handle_backtest_create(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_backtest_list(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_backtest_detail(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_backtest_equity(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_backtest_status(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    
    // 系统管理路由
    void setup_system_routes();
    void handle_system_status(const drogon::HttpRequestPtr& req,
                             std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void handle_system_metrics(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    
    // 工具方法
    drogon::HttpResponsePtr create_json_response(int status_code, const nlohmann::json& data);
    drogon::HttpResponsePtr create_error_response(int status_code, const std::string& message);
};

#endif