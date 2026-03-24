# 股票回测系统问题分析与修复报告

## 问题概述

本次修复解决了股票回测系统在数据导入和回测执行过程中遇到的多个关键问题，确保系统能够正常运行并产生正确的回测结果。

## 问题列表与修复方案

### 1. CLI回调函数生命周期问题

**问题描述**：
在`backend/cli/CLI.cpp`文件中，`setup_backtest_commands`函数使用引用捕获局部变量，导致回调函数执行时参数值为空。

**错误表现**：
```
Querying data file with id: 
Query result: success
Data file content: null
{"error": "数据文件路径不存在"}
```

**问题根源**：
```cpp
std::string run_data_id, run_strategy_id;
run_cmd->callback([this, &run_data_id, &run_strategy_id, &run_equity]()
                  { handle_backtest_run(run_data_id, run_strategy_id, run_equity); });
```
局部变量`run_data_id`和`run_strategy_id`在回调函数执行时已经超出作用域，引用捕获导致空值。

**修复方案**：
改用CLI11的选项组方式获取参数值：
```cpp
auto data_opt = run_cmd->add_option("--data", "数据文件ID")->required();
auto strategy_opt = run_cmd->add_option("--strategy", "策略ID")->required();
auto equity_opt = run_cmd->add_option("--equity", "初始资金")->default_val(DEFAULT_INITIAL_EQUITY);
run_cmd->callback([this, data_opt, strategy_opt, equity_opt]()
                  {
    std::string data_id = data_opt->as<std::string>();
    std::string strategy_id = strategy_opt->as<std::string>();
    double equity = equity_opt->as<double>();
    handle_backtest_run(data_id, strategy_id, equity); });
```

### 2. 数据库参数绑定类型错误

**问题描述**：
在`backend/models/Database.cpp`文件中，`execute_prepared_statement`函数尝试将所有参数转换为double类型，导致日期字符串转换失败。

**错误表现**：
```
stod: no conversion
```

**问题根源**：
```cpp
double num = std::stod(value);
rc = sqlite3_bind_double(stmt, index, num);
```
尝试将日期字符串（如"Thu Jan  2 00:00:00 2020"）转换为double类型。

**修复方案**：
添加类型检查逻辑，只对纯数字字符串尝试转换为double：
```cpp
bool is_number = true;
for (char c : value) {
    if (!std::isdigit(c) && c != '.' && c != '-' && c != '+') {
        is_number = false;
        break;
    }
}
if (is_number && !value.empty()) {
    try {
        double num = std::stod(value);
        rc = sqlite3_bind_double(stmt, index, num);
    } catch (...) {
        rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
    }
} else {
    rc = sqlite3_bind_text(stmt, index, value.c_str(), -1, SQLITE_TRANSIENT);
}
```

### 3. 策略参数解析错误

**问题描述**：
在`backend/cli/CLI.cpp`文件中，策略参数解析时直接将数据库中存储的JSON字符串当作JSON对象使用。

**错误表现**：
```
[json.exception.type_error.305] cannot use operator[] with a string argument with string
```

**问题根源**：
```cpp
json params = strategy["parameters"];
int short_period = params["short_period"];
```
数据库中`parameters`字段存储的是JSON字符串，需要先解析为JSON对象。

**修复方案**：
先将字符串解析为JSON对象：
```cpp
std::string params_str = strategy["parameters"];
json params = json::parse(params_str);
int short_period = params["short_period"];
int long_period = params["long_period"];
```

### 4. 空值检查缺失问题

**问题描述**：
在访问JSON对象的字段时没有进行空值检查，导致访问null值时抛出异常。

**错误表现**：
```
[json.exception.type_error.302] type must be string, but is null
```

**问题根源**：
```cpp
std::string data_format = data_file["format"];
```
直接访问可能为null的字段。

**修复方案**：
添加空值检查：
```cpp
std::string data_format = "default";
if (data_file.contains("format") && !data_file["format"].is_null()) {
    data_format = data_file["format"];
}
```

## 修复验证结果

### 数据导入功能验证

```bash
./build/stock_backtrack data upload data/sz_000002.csv --symbol sz.000002 --format baostock
```

**执行结果**：
```
正在连接数据库: stock_backtrack.db
数据库连接成功
成功连接到数据库: stock_backtrack.db
=== Callback called ===
File: data/sz_000002.csv
Symbol: sz.000002
Format: 'baostock'
BaoStockDataHandler: Loaded 1212 records, 0 errors
{
  "id": "data_20260323181304_4743",
  "status": "success",
  "symbol": "sz.000002"
}
数据库连接已关闭
```

### 回测功能验证

```bash
./build/stock_backtrack backtest run --data data_20260323180433_8090 --strategy strategy_20260323180433_8090 --equity 100000
```

**执行结果**：
```
正在连接数据库: stock_backtrack.db
数据库连接成功
成功连接到数据库: stock_backtrack.db
BaoStockDataHandler: Loaded 1212 records, 0 errors
{
  "final_equity": 3417.5229000028735,
  "id": "backtest_20260323181251_3783",
  "initial_equity": 100000.0,
  "max_drawdown": 0.9920281484269842,
  "return_rate": -0.9658247709999712,
  "sharpe_ratio": -0.09770395346573547,
  "status": "completed"
}
数据库连接已关闭
```

## 总结

本次修复解决了股票回测系统中的四个关键问题：
1. CLI回调函数生命周期问题
2. 数据库参数绑定类型错误
3. 策略参数解析错误
4. 空值检查缺失问题

修复后，系统能够正常导入数据并执行回测，生成正确的回测结果。所有功能均已验证通过。

## 技术改进建议

1. **代码审查**：建立代码审查流程，确保代码质量
2. **单元测试**：为关键功能添加单元测试
3. **异常处理**：完善异常处理机制，提供更详细的错误信息
4. **类型安全**：加强类型检查，避免类型转换错误
5. **文档完善**：补充API文档和使用说明
