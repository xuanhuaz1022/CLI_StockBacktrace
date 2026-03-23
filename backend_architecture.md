# 股票回测系统后端架构设计

## 1. 架构概述

本架构设计旨在将现有的C++股票回测系统转变为一个可通过多种方式访问的后端服务，提供完整的回测功能，并支持与前端界面的集成。系统采用纯C++实现，使用Drogon作为REST API框架，SQLite作为数据存储方案。

### 1.1 用户类型

本系统支持两种类型的用户：

| 用户类型           | 访问方式        | 特点          | 主要需求         |
| -------------- | ----------- | ----------- | ------------ |
| **人类用户**       | React Web前端 | 图形化界面，交互式操作 | 可视化、易用性、实时反馈 |
| **AI Agent用户** | CLI命令行      | 自动化调用，批处理能力 | 高效、简洁、结构化输出  |

### 1.2 设计原则

1. **统一核心**：两种用户共享相同的后端核心逻辑
2. **多样化接口**：提供RESTful API和CLI两种访问方式
3. **优化体验**：针对不同用户类型优化交互体验
4. **资源高效**：CLI方式更节约token和上下文资源

## 2. 技术栈选择

| 组件    | 技术     | 版本    | 选型理由                      |
| ----- | ------ | ----- | ------------------------- |
| 后端语言  | C++    | C++20 | 高性能，适合计算密集型任务，现有代码基础      |
| Web框架 | Drogon | 最新稳定版 | 高性能异步框架，支持现代C++，内置路由和参数解析 |
| 数据库   | SQLite | 3.x   | 轻量级，无需独立服务器，适合中小型应用       |
| 构建工具  | CMake  | 3.16+ | 跨平台，支持现代C++特性             |
| 依赖管理  | vcpkg  | 最新版   | 方便管理第三方库依赖                |
| CLI框架 | CLI11  | 最新版   | 现代C++命令行解析库，易于使用          |

## 3. 系统架构图

```
+-------------------+       +-------------------+       +-------------------+
|                   |       |                   |       |                   |
|  React Web前端    | <---> |   RESTful API     |       |                   |
|  (人类用户)       |       |   服务层          |       |                   |
|                   |       |                   |       |                   |
+-------------------+       +-------------------+       |                   |
                              |           |            |                   |
                              v           v            |                   |
                      +---------------+    |            |  核心回测引擎     |
                      |               |    |            |                   |
                      |  数据管理模块  |    +---------> |                   |
                      |               |                 |                   |
                      +---------------+                 +-------------------+
                              |                              |
                              v                              v
                      +---------------+             +-------------------+
                      |               |             |                   |
                      |  SQLite数据库  |             |  策略实现模块     |
                      |               |             |                   |
                      +---------------+             +-------------------+
                                                            ^
                                                            |
+-------------------+       +-------------------+            |
|                   |       |                   |            |
|  AI Agent用户     | <---> |   CLI接口层       |------------+
|                   |       |                   |
+-------------------+       +-------------------+
```

## 4. 模块设计

### 4.1 API服务层（RESTful）

- **路由管理**：处理HTTP请求，路由到相应的处理函数
- **请求处理**：解析请求参数，调用核心模块，返回响应
- **认证授权**：可选的用户认证机制
- **错误处理**：统一的错误响应格式
- **状态管理**：支持异步任务状态查询

### 4.2 CLI接口层

- **命令解析**：解析命令行参数和选项
- **参数验证**：验证输入参数的有效性
- **结果输出**：格式化输出结果（JSON或文本）
- **批处理支持**：支持批量执行多个任务
- **错误处理**：清晰的错误消息和退出码

### 4.3 核心回测引擎

- **回测引擎**：管理回测流程，协调各模块工作
- **策略管理**：加载和执行交易策略
- **执行处理**：模拟交易执行
- **绩效计算**：计算回测结果和绩效指标

### 4.4 数据管理模块

- **数据上传**：处理CSV文件上传
- **数据存储**：将数据存储到SQLite
- **数据检索**：查询和获取数据

### 4.5 数据处理模块

- **数据加载**：从文件或数据库加载数据
- **数据预处理**：清洗和转换数据
- **指标计算**：计算技术指标（如MA）

## 5. RESTful API接口设计（人类用户）

### 5.1 数据管理接口

| 方法     | 路径               | 功能        | 请求体                 | 成功响应                                                                                                                 |
| ------ | ---------------- | --------- | ------------------- | -------------------------------------------------------------------------------------------------------------------- |
| POST   | /api/data/upload | 上传CSV数据文件 | multipart/form-data | `{"id": "data_id", "symbol": "A_STOCK", "status": "success"}`                                                        |
| GET    | /api/data/list   | 列出可用的数据文件 | N/A                 | `[{"id": "data_id", "symbol": "A_STOCK", "upload_time": "2026-03-22T12:00:00Z"}]`                                    |
| GET    | /api/data/{id}   | 获取数据详情    | N/A                 | `{"id": "data_id", "symbol": "A_STOCK", "start_date": "2020-01-01", "end_date": "2023-12-31", "record_count": 1000}` |
| DELETE | /api/data/{id}   | 删除数据      | N/A                 | `{"status": "success", "message": "Data deleted"}`                                                                   |

### 5.2 策略管理接口

| 方法     | 路径                   | 功能     | 请求体                                                                          | 成功响应                                                                                              |
| ------ | -------------------- | ------ | ---------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------- |
| POST   | /api/strategies      | 创建策略   | `{"name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 10}}` | `{"id": "strategy_id", "name": "MA_Cross", "status": "success"}`                                  |
| GET    | /api/strategies      | 列出策略   | N/A                                                                          | `[{"id": "strategy_id", "name": "MA_Cross", "created_at": "2026-03-22T12:00:00Z"}]`               |
| GET    | /api/strategies/{id} | 获取策略详情 | N/A                                                                          | `{"id": "strategy_id", "name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 10}}` |
| PUT    | /api/strategies/{id} | 更新策略   | `{"name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 15}}` | `{"id": "strategy_id", "name": "MA_Cross", "status": "success"}`                                  |
| DELETE | /api/strategies/{id} | 删除策略   | N/A                                                                          | `{"status": "success", "message": "Strategy deleted"}`                                            |

### 5.3 回测管理接口

| 方法   | 路径                         | 功能     | 请求体                                                                                | 成功响应                                                                                                                                                                      |
| ---- | -------------------------- | ------ | ---------------------------------------------------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| POST | /api/backtests             | 运行回测   | `{"data_id": "data_id", "strategy_id": "strategy_id", "initial_equity": 100000.0}` | `{"id": "backtest_id", "status": "running"}`                                                                                                                              |
| GET  | /api/backtests             | 列出回测结果 | N/A                                                                                | `[{"id": "backtest_id", "strategy_name": "MA_Cross", "start_time": "2026-03-22T12:00:00Z", "status": "completed"}]`                                                       |
| GET  | /api/backtests/{id}        | 获取回测详情 | N/A                                                                                | `{"id": "backtest_id", "strategy_name": "MA_Cross", "initial_equity": 100000.0, "final_equity": 110000.0, "return_rate": 0.1, "max_drawdown": 0.05, "sharpe_ratio": 1.5}` |
| GET  | /api/backtests/{id}/equity | 获取权益曲线 | N/A                                                                                | `{"id": "backtest_id", "equity_curve": [100000.0, 101000.0, 102000.0, ...]}`                                                                                              |
| GET  | /api/backtests/{id}/status | 获取回测状态 | N/A                                                                                | `{"id": "backtest_id", "status": "completed", "progress": 100}`                                                                                                           |

### 5.4 系统管理接口

| 方法  | 路径                  | 功能     | 请求体 | 成功响应                                                                |
| --- | ------------------- | ------ | --- | ------------------------------------------------------------------- |
| GET | /api/system/status  | 获取系统状态 | N/A | `{"status": "healthy", "version": "1.0.0", "uptime": 3600}`         |
| GET | /api/system/metrics | 获取系统指标 | N/A | `{"cpu_usage": 25.5, "memory_usage": 512.0, "active_backtests": 2}` |

## 6. CLI接口设计（AI Agent用户）

### 6.1 CLI命令结构

```
stock_backtest <command> [options] [arguments]

Commands:
  data        数据管理操作
  strategy    策略管理操作
  backtest    回测执行操作
  system      系统管理操作
  help        显示帮助信息
```

### 6.2 数据管理命令

| 命令                   | 功能        | 示例                                                        | 输出格式 |
| -------------------- | --------- | --------------------------------------------------------- | ---- |
| `data upload <file>` | 上传CSV数据文件 | `stock_backtest data upload a_stock.csv --symbol A_STOCK` | JSON |
| `data list`          | 列出可用的数据文件 | `stock_backtest data list`                                | JSON |
| `data show <id>`     | 获取数据详情    | `stock_backtest data show data_123`                       | JSON |
| `data delete <id>`   | 删除数据      | `stock_backtest data delete data_123`                     | JSON |

### 6.3 策略管理命令

| 命令                       | 功能     | 示例                                                                                           | 输出格式 |
| ------------------------ | ------ | -------------------------------------------------------------------------------------------- | ---- |
| `strategy create <name>` | 创建策略   | `stock_backtest strategy create MA_Cross --params '{"short_period":5,"long_period":10}'`     | JSON |
| `strategy list`          | 列出策略   | `stock_backtest strategy list`                                                               | JSON |
| `strategy show <id>`     | 获取策略详情 | `stock_backtest strategy show strategy_123`                                                  | JSON |
| `strategy update <id>`   | 更新策略   | `stock_backtest strategy update strategy_123 --params '{"short_period":5,"long_period":15}'` | JSON |
| `strategy delete <id>`   | 删除策略   | `stock_backtest strategy delete strategy_123`                                                | JSON |

### 6.4 回测执行命令

| 命令                     | 功能     | 示例                                                                                    | 输出格式 |
| ---------------------- | ------ | ------------------------------------------------------------------------------------- | ---- |
| `backtest run`         | 运行回测   | `stock_backtest backtest run --data data_123 --strategy strategy_123 --equity 100000` | JSON |
| `backtest list`        | 列出回测结果 | `stock_backtest backtest list`                                                        | JSON |
| `backtest show <id>`   | 获取回测详情 | `stock_backtest backtest show backtest_123`                                           | JSON |
| `backtest equity <id>` | 获取权益曲线 | `stock_backtest backtest equity backtest_123`                                         | JSON |
| `backtest status <id>` | 获取回测状态 | `stock_backtest backtest status backtest_123`                                         | JSON |

### 6.5 系统管理命令

| 命令               | 功能     | 示例                              | 输出格式 |
| ---------------- | ------ | ------------------------------- | ---- |
| `system status`  | 获取系统状态 | `stock_backtest system status`  | JSON |
| `system metrics` | 获取系统指标 | `stock_backtest system metrics` | JSON |

### 6.6 全局选项

| 选项              | 功能                | 示例                                             |
| --------------- | ----------------- | ---------------------------------------------- |
| `--output, -o`  | 指定输出格式（json/text） | `stock_backtest data list --output json`       |
| `--quiet, -q`   | 静默模式，仅输出结果        | `stock_backtest backtest run --quiet`          |
| `--verbose, -v` | 详细输出模式            | `stock_backtest backtest run --verbose`        |
| `--config, -c`  | 指定配置文件            | `stock_backtest --config /path/to/config.json` |
| `--help, -h`    | 显示帮助信息            | `stock_backtest --help`                        |

### 6.7 批处理支持

```bash
# 批量执行回测任务
stock_backtest backtest batch --file tasks.json

# tasks.json 格式示例
{
  "tasks": [
    {
      "data_id": "data_123",
      "strategy_id": "strategy_456",
      "initial_equity": 100000
    },
    {
      "data_id": "data_789",
      "strategy_id": "strategy_012",
      "initial_equity": 200000
    }
  ]
}
```

## 7. 两种用户类型的对比与优化

### 7.1 交互方式对比

| 特性          | RESTful API（人类用户） | CLI（AI Agent用户） |
| ----------- | ----------------- | --------------- |
| **交互方式**    | HTTP请求/响应         | 命令行调用           |
| **状态管理**    | 支持异步任务状态查询        | 同步执行，立即返回结果     |
| **输出格式**    | JSON（结构化）         | JSON（结构化，可配置）   |
| **错误处理**    | HTTP状态码 + 错误消息    | 退出码 + 错误消息      |
| **批处理**     | 需要多次请求            | 原生支持批处理         |
| **资源消耗**    | 需要运行Web服务器        | 直接执行，资源消耗低      |
| **Token消耗** | 中等（需要处理HTTP头部）    | 低（命令简洁，输出结构化）   |
| **上下文管理**   | 需要保持会话状态          | 无状态，每次独立执行      |

### 7.2 针对AI Agent的优化

1. **结构化输出**：所有输出默认为JSON格式，便于AI解析
2. **简洁命令**：命令设计简洁明了，减少AI记忆负担
3. **批处理能力**：支持批量执行，减少交互次数
4. **错误处理**：明确的错误代码和消息，便于AI理解
5. **参数验证**：清晰的参数要求，减少AI猜测
6. **帮助文档**：完整的帮助信息，便于AI学习和使用

### 7.3 针对人类用户的优化

1. **图形化界面**：React前端提供直观的操作界面
2. **实时反馈**：支持异步任务状态实时更新
3. **数据可视化**：图表展示回测结果和权益曲线
4. **交互式操作**：表单输入、下拉选择等交互元素
5. **历史记录**：保存操作历史，便于回顾
6. **错误提示**：友好的错误提示和解决方案

## 8. 数据库设计

### 8.1 数据表结构

#### `data_files`表

| 字段名            | 数据类型       | 约束            | 描述     |
| -------------- | ---------- | ------------- | ------ |
| `id`           | `TEXT`     | `PRIMARY KEY` | 数据文件ID |
| `symbol`       | `TEXT`     | `NOT NULL`    | 股票代码   |
| `file_path`    | `TEXT`     | `NOT NULL`    | 文件路径   |
| `start_date`   | `DATETIME` | `NOT NULL`    | 数据开始日期 |
| `end_date`     | `DATETIME` | `NOT NULL`    | 数据结束日期 |
| `record_count` | `INTEGER`  | `NOT NULL`    | 记录数量   |
| `upload_time`  | `DATETIME` | `NOT NULL`    | 上传时间   |

#### `strategies`表

| 字段名          | 数据类型       | 约束            | 描述           |
| ------------ | ---------- | ------------- | ------------ |
| `id`         | `TEXT`     | `PRIMARY KEY` | 策略ID         |
| `name`       | `TEXT`     | `NOT NULL`    | 策略名称         |
| `type`       | `TEXT`     | `NOT NULL`    | 策略类型         |
| `parameters` | `TEXT`     | `NOT NULL`    | 策略参数（JSON格式） |
| `created_at` | `DATETIME` | `NOT NULL`    | 创建时间         |
| `updated_at` | `DATETIME` | `NOT NULL`    | 更新时间         |

#### `backtests`表

| 字段名              | 数据类型       | 约束                          | 描述                           |
| ---------------- | ---------- | --------------------------- | ---------------------------- |
| `id`             | `TEXT`     | `PRIMARY KEY`               | 回测ID                         |
| `data_id`        | `TEXT`     | `REFERENCES data_files(id)` | 数据文件ID                       |
| `strategy_id`    | `TEXT`     | `REFERENCES strategies(id)` | 策略ID                         |
| `initial_equity` | `REAL`     | `NOT NULL`                  | 初始资金                         |
| `final_equity`   | `REAL`     | <br />                      | 最终资金                         |
| `return_rate`    | `REAL`     | <br />                      | 收益率                          |
| `max_drawdown`   | `REAL`     | <br />                      | 最大回撤                         |
| `sharpe_ratio`   | `REAL`     | <br />                      | 夏普比率                         |
| `status`         | `TEXT`     | `NOT NULL`                  | 状态（running/completed/failed） |
| `progress`       | `INTEGER`  | `NOT NULL`                  | 进度（0-100）                    |
| `start_time`     | `DATETIME` | `NOT NULL`                  | 开始时间                         |
| `end_time`       | `DATETIME` | <br />                      | 结束时间                         |

#### `equity_curves`表

| 字段名           | 数据类型       | 约束                          | 描述   |
| ------------- | ---------- | --------------------------- | ---- |
| `id`          | `INTEGER`  | `PRIMARY KEY AUTOINCREMENT` | 记录ID |
| `backtest_id` | `TEXT`     | `REFERENCES backtests(id)`  | 回测ID |
| `timestamp`   | `DATETIME` | `NOT NULL`                  | 时间戳  |
| `equity`      | `REAL`     | `NOT NULL`                  | 权益值  |

### 8.2 数据存储策略

1. **原始数据**：CSV文件存储在文件系统中，数据库中仅保存文件路径和元数据
2. **策略配置**：存储在数据库中，参数以JSON格式保存
3. **回测结果**：存储在数据库中，权益曲线单独存储以提高查询性能
4. **缓存策略**：对于频繁访问的数据，使用内存缓存减少数据库访问

## 9. 部署方案

### 9.1 依赖安装

- **Drogon**：通过vcpkg安装
- **SQLite**：系统自带或通过vcpkg安装
- **CLI11**：通过vcpkg安装
- **其他依赖**：nlohmann/json（JSON处理）

### 9.2 构建流程

1. 配置CMake
2. 编译项目
3. 生成可执行文件（支持API服务和CLI两种模式）

### 9.3 运行环境

- **操作系统**：支持Linux、macOS、Windows
- **硬件要求**：根据数据量和并发需求调整
- **端口配置**：默认8080端口，可通过环境变量或配置文件修改

### 9.4 运行模式

```bash
# 启动API服务（供Web前端使用）
./stock_backtest --mode api --port 8080

# CLI模式（供AI Agent使用）
./stock_backtest data list
./stock_backtest backtest run --data data_123 --strategy strategy_456

# 显示帮助
./stock_backtest --help
```

### 9.5 容器化

```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libsqlite3-dev \
    wget \
    git

# 安装vcpkg
RUN git clone https://github.com/microsoft/vcpkg.git /vcpkg
RUN /vcpkg/bootstrap-vcpkg.sh

# 安装依赖
RUN /vcpkg/vcpkg install drogon nlohmann-json cli11

WORKDIR /app

COPY . /app

# 构建项目
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake && \
    cmake --build .

# 运行服务
CMD ["/app/build/stock_backtest", "--mode", "api"]
```

## 10. 性能优化策略

1. **异步处理**：使用Drogon的异步特性处理请求
2. **线程池**：利用现有线程池处理回测任务
3. **缓存**：对频繁访问的数据和计算结果进行缓存
4. **数据库优化**：合理设计索引，使用事务批量操作
5. **内存管理**：避免内存泄漏，合理使用智能指针
6. **CLI优化**：减少不必要的输出，优化启动速度

## 11. 安全考虑

1. **输入验证**：对所有API和CLI输入进行严格验证
2. **文件上传安全**：限制文件大小和类型，防止恶意文件上传
3. **SQL注入防护**：使用参数化查询
4. **错误处理**：不向客户端暴露详细错误信息
5. **日志记录**：记录关键操作和错误信息
6. **权限控制**：可选的用户认证和授权机制

## 12. 扩展性设计

1. **模块化**：各模块间低耦合，便于扩展
2. **插件系统**：支持自定义策略和指标
3. **配置管理**：使用配置文件管理系统参数
4. **监控系统**：集成监控和告警机制
5. **API版本控制**：支持API版本演进
6. **CLI扩展**：易于添加新命令和选项

## 13. 开发计划

1. **阶段1**：搭建基础框架，实现CLI接口层
2. **阶段2**：集成现有回测引擎，实现核心功能
3. **阶段3**：实现RESTful API服务层
4. **阶段4**：实现数据持久化和管理功能
5. **阶段5**：优化性能和安全性
6. **阶段6**：测试和部署

## 14. 总结

本架构设计提供了一个完整的后端服务方案，将现有的C++股票回测系统转变为一个支持多种访问方式的服务。系统同时支持React Web前端（人类用户）和CLI命令行（AI Agent用户）两种访问方式，采用Drogon框架和SQLite数据库，既保证了系统的高性能，又简化了部署和维护。

通过合理的模块划分和接口设计，系统具有良好的扩展性和可维护性。CLI接口针对AI Agent用户进行了优化，具有低token消耗、简洁命令、结构化输出等特点，能够满足AI自动化调用的需求。RESTful API则为人类用户提供了友好的图形化操作界面，支持实时状态更新和数据可视化。

两种访问方式共享相同的后端核心逻辑，确保了功能的一致性和代码的可维护性，能够满足不同类型用户的需求。
