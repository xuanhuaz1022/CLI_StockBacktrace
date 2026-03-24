#include "APIServer.h"
#include <nlohmann/json.hpp>
#include <drogon/HttpResponse.h>
#include <drogon/HttpTypes.h>
#include <drogon/HttpRequest.h>
#include <iostream>
#include <stdexcept>

using json = nlohmann::json;

APIServer::APIServer() : app_(drogon::HttpAppFramework::instance()) {
    setup_routes();
}

APIServer::~APIServer() {
    stop();
}

bool APIServer::start(int port) {
    try {
        app_.addListener("0.0.0.0", port);
        app_.enableSession();
        app_.setThreadNum(4);
        app_.run();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start API server: " << e.what() << std::endl;
        return false;
    }
}

void APIServer::stop() {
    app_.stop();
}

void APIServer::setup_routes() {
    setup_data_routes();
    setup_strategy_routes();
    setup_backtest_routes();
    setup_system_routes();
}

void APIServer::setup_data_routes() {
    app_.registerHandler("/api/data/upload", [this](const auto& req, auto&& callback) {
        handle_data_upload(req, std::move(callback));
    }, {drogon::Post});
    
    app_.registerHandler("/api/data/list", [this](const auto& req, auto&& callback) {
        handle_data_list(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/data/{id}", [this](const auto& req, auto&& callback) {
        handle_data_detail(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/data/{id}", [this](const auto& req, auto&& callback) {
        handle_data_delete(req, std::move(callback));
    }, {drogon::Delete});
}

void APIServer::handle_data_upload(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (req->getFiles().empty()) {
            callback(create_error_response(400, "No file uploaded"));
            return;
        }
        
        const auto& file = req->getFiles()[0];
        if (file.fileName.empty()) {
            callback(create_error_response(400, "Empty file name"));
            return;
        }
        
        json response = {
            {"id", "data_id"},
            {"symbol", "A_STOCK"},
            {"status", "success"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_data_list(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        json response = {
            {
                {"id", "data_id"},
                {"symbol", "A_STOCK"},
                {"upload_time", "2026-03-22T12:00:00Z"}
            }
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_data_detail(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"symbol", "A_STOCK"},
            {"start_date", "2020-01-01"},
            {"end_date", "2023-12-31"},
            {"record_count", 1000}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_data_delete(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"status", "success"},
            {"message", "Data deleted"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::setup_strategy_routes() {
    app_.registerHandler("/api/strategies", [this](const auto& req, auto&& callback) {
        handle_strategy_create(req, std::move(callback));
    }, {drogon::Post});
    
    app_.registerHandler("/api/strategies", [this](const auto& req, auto&& callback) {
        handle_strategy_list(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/strategies/{id}", [this](const auto& req, auto&& callback) {
        handle_strategy_detail(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/strategies/{id}", [this](const auto& req, auto&& callback) {
        handle_strategy_update(req, std::move(callback));
    }, {drogon::Put});
    
    app_.registerHandler("/api/strategies/{id}", [this](const auto& req, auto&& callback) {
        handle_strategy_delete(req, std::move(callback));
    }, {drogon::Delete});
}

void APIServer::handle_strategy_create(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!req->getJsonObject()) {
            callback(create_error_response(400, "Invalid JSON format"));
            return;
        }
        
        auto json_obj = req->getJsonObject();
        if (!json_obj->contains("name") || !(*json_obj)["name"].is_string()) {
            callback(create_error_response(400, "Missing or invalid 'name' field"));
            return;
        }
        
        if (!json_obj->contains("parameters") || !(*json_obj)["parameters"].is_object()) {
            callback(create_error_response(400, "Missing or invalid 'parameters' field"));
            return;
        }
        
        json response = {
            {"id", "strategy_id"},
            {"name", (*json_obj)["name"]},
            {"status", "success"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_strategy_list(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        json response = {
            {
                {"id", "strategy_id"},
                {"name", "MA_Cross"},
                {"created_at", "2026-03-22T12:00:00Z"}
            }
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_strategy_detail(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"name", "MA_Cross"},
            {"parameters", {
                {"short_period", 5},
                {"long_period", 10}
            }}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_strategy_update(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        if (!req->getJsonObject()) {
            callback(create_error_response(400, "Invalid JSON format"));
            return;
        }
        
        auto json_obj = req->getJsonObject();
        if (!json_obj->contains("name") || !(*json_obj)["name"].is_string()) {
            callback(create_error_response(400, "Missing or invalid 'name' field"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"name", (*json_obj)["name"]},
            {"status", "success"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_strategy_delete(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"status", "success"},
            {"message", "Strategy deleted"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::setup_backtest_routes() {
    app_.registerHandler("/api/backtests", [this](const auto& req, auto&& callback) {
        handle_backtest_create(req, std::move(callback));
    }, {drogon::Post});
    
    app_.registerHandler("/api/backtests", [this](const auto& req, auto&& callback) {
        handle_backtest_list(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/backtests/{id}", [this](const auto& req, auto&& callback) {
        handle_backtest_detail(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/backtests/{id}/equity", [this](const auto& req, auto&& callback) {
        handle_backtest_equity(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/backtests/{id}/status", [this](const auto& req, auto&& callback) {
        handle_backtest_status(req, std::move(callback));
    }, {drogon::Get});
}

void APIServer::handle_backtest_create(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        if (!req->getJsonObject()) {
            callback(create_error_response(400, "Invalid JSON format"));
            return;
        }
        
        auto json_obj = req->getJsonObject();
        if (!json_obj->contains("data_id") || !(*json_obj)["data_id"].is_string()) {
            callback(create_error_response(400, "Missing or invalid 'data_id' field"));
            return;
        }
        
        if (!json_obj->contains("strategy_id") || !(*json_obj)["strategy_id"].is_string()) {
            callback(create_error_response(400, "Missing or invalid 'strategy_id' field"));
            return;
        }
        
        if (!json_obj->contains("initial_equity") || !(*json_obj)["initial_equity"].is_number()) {
            callback(create_error_response(400, "Missing or invalid 'initial_equity' field"));
            return;
        }
        
        json response = {
            {"id", "backtest_id"},
            {"status", "running"}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_backtest_list(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        json response = {
            {
                {"id", "backtest_id"},
                {"strategy_name", "MA_Cross"},
                {"start_time", "2026-03-22T12:00:00Z"},
                {"status", "completed"}
            }
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_backtest_detail(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"strategy_name", "MA_Cross"},
            {"initial_equity", 100000.0},
            {"final_equity", 110000.0},
            {"return_rate", 0.1},
            {"max_drawdown", 0.05},
            {"sharpe_ratio", 1.5}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_backtest_equity(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"equity_curve", {100000.0, 101000.0, 102000.0}}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_backtest_status(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        auto params = req->getParameters();
        if (!params.count("id")) {
            callback(create_error_response(400, "Missing 'id' parameter"));
            return;
        }
        
        json response = {
            {"id", params["id"]},
            {"status", "completed"},
            {"progress", 100}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::setup_system_routes() {
    app_.registerHandler("/api/system/status", [this](const auto& req, auto&& callback) {
        handle_system_status(req, std::move(callback));
    }, {drogon::Get});
    
    app_.registerHandler("/api/system/metrics", [this](const auto& req, auto&& callback) {
        handle_system_metrics(req, std::move(callback));
    }, {drogon::Get});
}

void APIServer::handle_system_status(const drogon::HttpRequestPtr& req,
                                  std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        json response = {
            {"status", "healthy"},
            {"version", "1.0.0"},
            {"uptime", 3600}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

void APIServer::handle_system_metrics(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    try {
        json response = {
            {"cpu_usage", 25.5},
            {"memory_usage", 512.0},
            {"active_backtests", 2}
        };
        callback(create_json_response(200, response));
    } catch (const std::exception& e) {
        callback(create_error_response(500, std::string("Internal server error: ") + e.what()));
    }
}

drogon::HttpResponsePtr APIServer::create_json_response(int status_code, const json& data) {
    auto resp = drogon::HttpResponse::newHttpResponse();
    resp->setStatusCode(static_cast<drogon::HttpStatusCode>(status_code));
    resp->setContentTypeCode(drogon::CT_JSON);
    resp->setBody(data.dump());
    return resp;
}

drogon::HttpResponsePtr APIServer::create_error_response(int status_code, const std::string& message) {
    json error_data = {
        {"error", message},
        {"status", "error"}
    };
    return create_json_response(status_code, error_data);
}