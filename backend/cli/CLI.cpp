#include "CLI.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <random>
#include <filesystem>
#include "../../core/include/data_handler.h"
#include "../../core/include/strategy.h"

namespace stock_backtrack
{

    // 常量定义
    constexpr double DEFAULT_INITIAL_EQUITY = 100000.0;
    constexpr int BACKTEST_RUNNING_PROGRESS = 50;
    constexpr int BACKTEST_COMPLETED_PROGRESS = 100;
    constexpr int RANDOM_ID_MIN = 1000;
    constexpr int RANDOM_ID_MAX = 9999;

    CLI::CLI() : engine_(nullptr)
    {
        // 初始化数据库连接
        if (!db.connect())
        {
            throw std::runtime_error("数据库连接失败");
        }

        // 设置应用信息
        app.name("stock_backtrack");
        app.description("股票回测系统命令行工具");

        // 设置全局选项
        std::string output_format;
        app.add_option("-o,--output", output_format)->description("指定输出格式（json/text）");

        app.add_flag("-q,--quiet")->description("静默模式，仅输出结果");
        app.add_flag("-v,--verbose")->description("详细输出模式");
        std::string config_file;
        app.add_option("-c,--config", config_file)->description("指定配置文件");

        // 设置子命令
        setup_data_commands();
        setup_strategy_commands();
        setup_backtest_commands();
        setup_system_commands();
    }

    CLI::~CLI()
    {
        db.disconnect();
        if (engine_ != nullptr)
        {
            delete engine_;
            engine_ = nullptr;
        }
    }

    int CLI::run(int argc, char **argv)
    {
        try
        {
            app.parse(argc, argv);
            return 0;
        }
        catch (const ::CLI::ParseError &e)
        {
            return app.exit(e);
        }
    }

    void CLI::setup_data_commands()
    {
        auto data = app.add_subcommand("data", "数据管理操作");

        auto upload_cmd = data->add_subcommand("upload", "上传CSV数据文件");
        std::string upload_file, upload_symbol;
        upload_cmd->add_option("<file>", upload_file)->required();
        upload_cmd->add_option("--symbol", upload_symbol)->required();
        upload_cmd->callback([this, &upload_file, &upload_symbol]()
                             { handle_data_upload(upload_file, upload_symbol); });

        data->add_subcommand("list", "列出可用的数据文件")
            ->callback([this]()
                       { handle_data_list(); });

        auto show_cmd = data->add_subcommand("show", "获取数据详情");
        std::string show_id;
        show_cmd->add_option("<id>", show_id)->required();
        show_cmd->callback([this, &show_id]()
                           { handle_data_show(show_id); });

        auto delete_cmd = data->add_subcommand("delete", "删除数据");
        std::string delete_id;
        delete_cmd->add_option("<id>", delete_id)->required();
        delete_cmd->callback([this, &delete_id]()
                             { handle_data_delete(delete_id); });
    }

    void CLI::setup_strategy_commands()
    {
        auto strategy = app.add_subcommand("strategy", "策略管理操作");

        auto create_cmd = strategy->add_subcommand("create", "创建策略");
        std::string create_name, create_params;
        create_cmd->add_option("<name>", create_name)->required();
        create_cmd->add_option("--params", create_params)->required();
        create_cmd->callback([this, &create_name, &create_params]()
                             { handle_strategy_create(create_name, create_params); });

        strategy->add_subcommand("list", "列出策略")
            ->callback([this]()
                       { handle_strategy_list(); });

        auto show_strategy_cmd = strategy->add_subcommand("show", "获取策略详情");
        std::string show_strategy_id;
        show_strategy_cmd->add_option("<id>", show_strategy_id)->required();
        show_strategy_cmd->callback([this, &show_strategy_id]()
                                    { handle_strategy_show(show_strategy_id); });

        auto update_cmd = strategy->add_subcommand("update", "更新策略");
        std::string update_id, update_name, update_params;
        update_cmd->add_option("<id>", update_id)->required();
        update_cmd->add_option("--name", update_name);
        update_cmd->add_option("--params", update_params);
        update_cmd->callback([this, &update_id, &update_name, &update_params]()
                             { handle_strategy_update(update_id, update_name, update_params); });

        auto delete_strategy_cmd = strategy->add_subcommand("delete", "删除策略");
        std::string delete_strategy_id;
        delete_strategy_cmd->add_option("<id>", delete_strategy_id)->required();
        delete_strategy_cmd->callback([this, &delete_strategy_id]()
                                      { handle_strategy_delete(delete_strategy_id); });
    }

    void CLI::setup_backtest_commands()
    {
        auto backtest = app.add_subcommand("backtest", "回测执行操作");

        auto run_cmd = backtest->add_subcommand("run", "运行回测");
        std::string run_data_id, run_strategy_id;
        double run_equity = DEFAULT_INITIAL_EQUITY;
        run_cmd->add_option("--data", run_data_id)->required();
        run_cmd->add_option("--strategy", run_strategy_id)->required();
        run_cmd->add_option("--equity", run_equity)->default_val(DEFAULT_INITIAL_EQUITY);
        run_cmd->callback([this, &run_data_id, &run_strategy_id, &run_equity]()
                          { handle_backtest_run(run_data_id, run_strategy_id, run_equity); });

        backtest->add_subcommand("list", "列出回测结果")
            ->callback([this]()
                       { handle_backtest_list(); });

        auto show_backtest_cmd = backtest->add_subcommand("show", "获取回测详情");
        std::string show_backtest_id;
        show_backtest_cmd->add_option("<id>", show_backtest_id)->required();
        show_backtest_cmd->callback([this, &show_backtest_id]()
                                    { handle_backtest_show(show_backtest_id); });

        auto equity_cmd = backtest->add_subcommand("equity", "获取权益曲线");
        std::string equity_id;
        equity_cmd->add_option("<id>", equity_id)->required();
        equity_cmd->callback([this, &equity_id]()
                             { handle_backtest_equity(equity_id); });

        auto status_cmd = backtest->add_subcommand("status", "获取回测状态");
        std::string status_id;
        status_cmd->add_option("<id>", status_id)->required();
        status_cmd->callback([this, &status_id]()
                             { handle_backtest_status(status_id); });

        auto batch_cmd = backtest->add_subcommand("batch", "批量执行回测");
        std::string batch_file;
        batch_cmd->add_option("--file", batch_file)->required();
        batch_cmd->callback([this, &batch_file]()
                            { handle_backtest_batch(batch_file); });
    }

    void CLI::setup_system_commands()
    {
        auto system = app.add_subcommand("system", "系统管理操作");

        system->add_subcommand("status", "获取系统状态")
            ->callback([this]()
                       { handle_system_status(); });

        system->add_subcommand("metrics", "获取系统指标")
            ->callback([this]()
                       { handle_system_metrics(); });
    }

    void CLI::handle_data_upload(const std::string &file, const std::string &symbol)
    {
        try
        {
            // 验证文件路径安全性
            if (!validate_file_path(file))
            {
                print_error("不安全的文件路径: " + file);
                return;
            }

            // 检查文件是否存在
            if (!std::filesystem::exists(file))
            {
                print_error("文件不存在: " + file);
                return;
            }

            // 读取文件获取数据信息
            DataHandler handler;
            handler.load_data(file, symbol);

            // 获取数据时间范围
            auto time_range = handler.get_time_range(symbol);
            auto start_time = time_range.first;
            auto end_time = time_range.second;

            // 获取数据记录
            auto bars = handler.get_bars(symbol, start_time, end_time);
            if (bars.empty())
            {
                print_error("数据文件为空");
                return;
            }

            // 获取数据范围（转换time_point为字符串）
            std::time_t start_t = std::chrono::system_clock::to_time_t(bars.front().timestamp);
            std::time_t end_t = std::chrono::system_clock::to_time_t(bars.back().timestamp);
            std::string start_date = std::ctime(&start_t);
            std::string end_date = std::ctime(&end_t);

            // 去掉换行符
            start_date.pop_back();
            end_date.pop_back();

            int record_count = bars.size();

            // 生成数据文件ID
            std::string id = generate_id("data");

            // 保存到数据库
            if (db.create_data_file(id, symbol, file, start_date, end_date, record_count))
            {
                json result = {
                    {"id", id},
                    {"symbol", symbol},
                    {"status", "success"}};
                print_json(result);
            }
            else
            {
                print_error("数据文件保存失败");
            }
        }
        catch (const std::exception &e)
        {
            print_error("上传失败: " + std::string(e.what()));
        }
    }

    void CLI::handle_data_list()
    {
        std::vector<json> data_files;
        if (db.get_data_files(data_files))
        {
            print_json(data_files);
        }
        else
        {
            print_error("获取数据文件列表失败");
        }
    }

    void CLI::handle_data_show(const std::string &id)
    {
        json data_file;
        if (db.get_data_file(id, data_file))
        {
            print_json(data_file);
        }
        else
        {
            print_error("数据文件不存在");
        }
    }

    void CLI::handle_data_delete(const std::string &id)
    {
        if (db.delete_data_file(id))
        {
            json result = {
                {"status", "success"},
                {"message", "Data deleted"}};
            print_json(result);
        }
        else
        {
            print_error("删除数据文件失败");
        }
    }

    void CLI::handle_strategy_create(const std::string &name, const std::string &params)
    {
        try
        {
            // 验证JSON格式
            json parameters = json::parse(params);

            // 生成策略ID
            std::string id = generate_id("strategy");

            // 保存到数据库
            if (db.create_strategy(id, name, "MA_Cross", params))
            {
                json result = {
                    {"id", id},
                    {"name", name},
                    {"status", "success"}};
                print_json(result);
            }
            else
            {
                print_error("策略创建失败");
            }
        }
        catch (const json::parse_error &e)
        {
            print_error("JSON格式错误: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            print_error("策略创建失败: " + std::string(e.what()));
        }
    }

    void CLI::handle_strategy_list()
    {
        std::vector<json> strategies;
        if (db.get_strategies(strategies))
        {
            print_json(strategies);
        }
        else
        {
            print_error("获取策略列表失败");
        }
    }

    void CLI::handle_strategy_show(const std::string &id)
    {
        json strategy;
        if (db.get_strategy(id, strategy))
        {
            print_json(strategy);
        }
        else
        {
            print_error("策略不存在");
        }
    }

    void CLI::handle_strategy_update(const std::string &id, const std::string &name, const std::string &params)
    {
        try
        {
            // 验证JSON格式
            if (!params.empty())
            {
                json temp = json::parse(params);
            }

            if (db.update_strategy(id, name, params))
            {
                json result = {
                    {"id", id},
                    {"name", name},
                    {"status", "success"}};
                print_json(result);
            }
            else
            {
                print_error("策略更新失败");
            }
        }
        catch (const json::parse_error &e)
        {
            print_error("JSON格式错误: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            print_error("策略更新失败: " + std::string(e.what()));
        }
    }

    void CLI::handle_strategy_delete(const std::string &id)
    {
        if (db.delete_strategy(id))
        {
            json result = {
                {"status", "success"},
                {"message", "Strategy deleted"}};
            print_json(result);
        }
        else
        {
            print_error("删除策略失败");
        }
    }

    void CLI::handle_backtest_run(const std::string &data_id, const std::string &strategy_id, double equity)
    {
        try
        {
            // 获取数据文件信息
            json data_file;
            if (!db.get_data_file(data_id, data_file))
            {
                print_error("数据文件不存在");
                return;
            }

            // 获取策略信息
            json strategy;
            if (!db.get_strategy(strategy_id, strategy))
            {
                print_error("策略不存在");
                return;
            }

            // 生成回测ID
            std::string backtest_id = generate_id("backtest");

            // 创建回测记录
            if (!db.create_backtest(backtest_id, data_id, strategy_id, equity))
            {
                print_error("创建回测记录失败");
                return;
            }

            // 加载数据
            DataHandler handler;
            try
            {
                handler.load_data(data_file["file_path"], data_file["symbol"]);
            }
            catch (const std::exception &e)
            {
                db.update_backtest_status(backtest_id, "failed", 0);
                print_error("数据加载失败: " + std::string(e.what()));
                return;
            }

            // 创建策略实例
            json params = strategy["parameters"];
            int short_period = params["short_period"];
            int long_period = params["long_period"];

            MovingAverageCrossStrategy ma_strategy(short_period, long_period);

            // 创建执行处理器
            ExecutionHandler executor(equity);

            // 创建回测引擎
            BacktestEngine backtest_engine(&handler, &ma_strategy, &executor);

            // 获取数据时间范围
            auto time_range = handler.get_time_range(data_file["symbol"]);

            // 运行回测
            db.update_backtest_status(backtest_id, "running", BACKTEST_RUNNING_PROGRESS);
            backtest_engine.run(data_file["symbol"], time_range.first, time_range.second);

            // 获取回测结果
            const auto &result = backtest_engine.get_result();

            // 更新回测结果
            db.update_backtest_status(backtest_id, "completed", BACKTEST_COMPLETED_PROGRESS,
                                      result.final_equity, result.return_rate,
                                      result.max_drawdown, result.sharpe_ratio);

            // 保存权益曲线
            for (size_t i = 0; i < result.equity_curve.size(); ++i)
            {
                std::string timestamp = std::to_string(i);
                db.add_equity_point(backtest_id, timestamp, result.equity_curve[i]);
            }

            json response = {
                {"id", backtest_id},
                {"status", "completed"},
                {"initial_equity", equity},
                {"final_equity", result.final_equity},
                {"return_rate", result.return_rate},
                {"max_drawdown", result.max_drawdown},
                {"sharpe_ratio", result.sharpe_ratio}};

            print_json(response);
        }
        catch (const std::exception &e)
        {
            print_error("回测失败: " + std::string(e.what()));
        }
    }

    void CLI::handle_backtest_list()
    {
        std::vector<json> backtests;
        if (db.get_backtests(backtests))
        {
            print_json(backtests);
        }
        else
        {
            print_error("获取回测列表失败");
        }
    }

    void CLI::handle_backtest_show(const std::string &id)
    {
        json backtest;
        if (db.get_backtest(id, backtest))
        {
            print_json(backtest);
        }
        else
        {
            print_error("回测不存在");
        }
    }

    void CLI::handle_backtest_equity(const std::string &id)
    {
        std::vector<json> equity_curve;
        if (db.get_equity_curve(id, equity_curve))
        {
            json result = {
                {"id", id},
                {"equity_curve", json::array()}};

            for (const auto &point : equity_curve)
            {
                result["equity_curve"].push_back(point["equity"]);
            }

            print_json(result);
        }
        else
        {
            print_error("获取权益曲线失败");
        }
    }

    void CLI::handle_backtest_status(const std::string &id)
    {
        json backtest;
        if (db.get_backtest(id, backtest))
        {
            json result = {
                {"id", id},
                {"status", backtest["status"]},
                {"progress", backtest["progress"]}};
            print_json(result);
        }
        else
        {
            print_error("回测不存在");
        }
    }

    void CLI::handle_backtest_batch(const std::string &file)
    {
        try
        {
            // 验证文件路径安全性
            if (!validate_file_path(file))
            {
                print_error("不安全的文件路径: " + file);
                return;
            }

            std::ifstream batch_file(file);
            if (!batch_file.is_open())
            {
                print_error("批处理文件不存在");
                return;
            }

            json tasks = json::parse(batch_file);

            for (const auto &task : tasks["tasks"])
            {
                std::string data_id = task["data_id"];
                std::string strategy_id = task["strategy_id"];
                double equity = task["initial_equity"];

                handle_backtest_run(data_id, strategy_id, equity);
            }
        }
        catch (const json::parse_error &e)
        {
            print_error("批处理文件格式错误: " + std::string(e.what()));
        }
        catch (const std::exception &e)
        {
            print_error("批处理执行失败: " + std::string(e.what()));
        }
    }

    void CLI::handle_system_status()
    {
        json status = {
            {"status", "healthy"},
            {"version", "1.0.0"},
            {"uptime", 0}};
        print_json(status);
    }

    void CLI::handle_system_metrics()
    {
        json metrics = {
            {"cpu_usage", 0.0},
            {"memory_usage", 0.0},
            {"active_backtests", 0}};
        print_json(metrics);
    }

    std::string CLI::generate_id(const std::string &prefix)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(RANDOM_ID_MIN, RANDOM_ID_MAX);

        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y%m%d%H%M%S");

        return prefix + "_" + ss.str() + "_" + std::to_string(dist(gen));
    }

    void CLI::print_json(const json &data)
    {
        std::cout << data.dump(2) << std::endl;
    }

    void CLI::print_error(const std::string &message)
    {
        std::cerr << "{\"error\": \"" << message << "\"}" << std::endl;
    }

    bool CLI::validate_file_path(const std::string &path)
    {
        try
        {
            std::filesystem::path fs_path(path);

            // 获取绝对路径
            if (!fs_path.is_absolute())
            {
                fs_path = std::filesystem::canonical(std::filesystem::current_path() / fs_path);
            }
            else
            {
                fs_path = std::filesystem::canonical(fs_path);
            }

            // 获取当前工作目录的绝对路径
            std::filesystem::path current_dir = std::filesystem::canonical(std::filesystem::current_path());

            // 检查路径是否在当前工作目录或其子目录中
            return fs_path.string().starts_with(current_dir.string());
        }
        catch (const std::exception &e)
        {
            return false;
        }
    }

} // namespace stock_backtrack
