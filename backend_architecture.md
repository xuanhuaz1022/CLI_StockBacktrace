# 股票回测系统后端架构设计

## 1. 架构概述

本架构设计旨在将现有的C++股票回测系统转变为一个可通过API访问的后端服务，提供完整的回测功能，并支持与前端界面的集成。系统采用纯C++实现，使用Drogon作为REST API框架，SQLite作为数据存储方案。

## 2. 技术栈选择

| 组件 | 技术 | 版本 | 选型理由 |
|------|------|------|----------|
| 后端语言 | C++ | C++20 | 高性能，适合计算密集型任务，现有代码基础 |
| Web框架 | Drogon | 最新稳定版 | 高性能异步框架，支持现代C++，内置路由和参数解析 |
| 数据库 | SQLite | 3.x | 轻量级，无需独立服务器，适合中小型应用 |
| 构建工具 | CMake | 3.16+ | 跨平台，支持现代C++特性 |
| 依赖管理 | vcpkg | 最新版 | 方便管理第三方库依赖 |

## 3. 系统架构图

```
+-------------------+       +-------------------+       +-------------------+
|                   |       |                   |       |                   |
|    前端界面       | <---> |    API服务层      | <---> |  核心回测引擎     |
|                   |       |                   |       |                   |
+-------------------+       +-------------------+       +-------------------+
                              |           |                      |
                              v           v                      v
                      +---------------+    |             +-------------------+
                      |               |    |             |                   |
                      |  数据管理模块  |    +----------> |  数据处理模块     |
                      |               |                 |                   |
                      +---------------+                 +-------------------+
                              |                              |
                              v                              v
                      +---------------+             +-------------------+
                      |               |             |                   |
                      |  SQLite数据库  |             |  策略实现模块     |
                      |               |             |                   |
                      +---------------+             +-------------------+
```

## 4. 模块设计

### 4.1 API服务层

- **路由管理**：处理HTTP请求，路由到相应的处理函数
- **请求处理**：解析请求参数，调用核心模块，返回响应
- **认证授权**：可选的用户认证机制
- **错误处理**：统一的错误响应格式

### 4.2 核心回测引擎

- **回测引擎**：管理回测流程，协调各模块工作
- **策略管理**：加载和执行交易策略
- **执行处理**：模拟交易执行
- **绩效计算**：计算回测结果和绩效指标

### 4.3 数据管理模块

- **数据上传**：处理CSV文件上传
- **数据存储**：将数据存储到SQLite
- **数据检索**：查询和获取数据

### 4.4 数据处理模块

- **数据加载**：从文件或数据库加载数据
- **数据预处理**：清洗和转换数据
- **指标计算**：计算技术指标（如MA）

## 5. API接口设计

### 5.1 数据管理接口

| 方法 | 路径 | 功能 | 请求体 | 成功响应 |
|------|------|------|--------|----------|
| POST | /api/data/upload | 上传CSV数据文件 | multipart/form-data | `{"id": "data_id", "symbol": "A_STOCK", "status": "success"}` |
| GET | /api/data/list | 列出可用的数据文件 | N/A | `[{"id": "data_id", "symbol": "A_STOCK", "upload_time": "2026-03-22T12:00:00Z"}]` |
| GET | /api/data/{id} | 获取数据详情 | N/A | `{"id": "data_id", "symbol": "A_STOCK", "start_date": "2020-01-01", "end_date": "2023-12-31", "record_count": 1000}` |
| DELETE | /api/data/{id} | 删除数据 | N/A | `{"status": "success", "message": "Data deleted"}` |

### 5.2 策略管理接口

| 方法 | 路径 | 功能 | 请求体 | 成功响应 |
|------|------|------|--------|----------|
| POST | /api/strategies | 创建策略 | `{"name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 10}}` | `{"id": "strategy_id", "name": "MA_Cross", "status": "success"}` |
| GET | /api/strategies | 列出策略 | N/A | `[{"id": "strategy_id", "name": "MA_Cross", "created_at": "2026-03-22T12:00:00Z"}]` |
| GET | /api/strategies/{id} | 获取策略详情 | N/A | `{"id": "strategy_id", "name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 10}}` |
| PUT | /api/strategies/{id} | 更新策略 | `{"name": "MA_Cross", "parameters": {"short_period": 5, "long_period": 15}}` | `{"id": "strategy_id", "name": "MA_Cross", "status": "success"}` |
| DELETE | /api/strategies/{id} | 删除策略 | N/A | `{"status": "success", "message": "Strategy deleted"}` |

### 5.3 回测管理接口

| 方法 | 路径 | 功能 | 请求体 | 成功响应 |
|------|------|------|--------|----------|
| POST | /api/backtests | 运行回测 | `{"data_id": "data_id", "strategy_id": "strategy_id", "initial_equity": 100000.0}` | `{"id": "backtest_id", "status": "running"}` |
| GET | /api/backtests | 列出回测结果 | N/A | `[{"id": "backtest_id", "strategy_name": "MA_Cross", "start_time": "2026-03-22T12:00:00Z", "status": "completed"}]` |
| GET | /api/backtests/{id} | 获取回测详情 | N/A | `{"id": "backtest_id", "strategy_name": "MA_Cross", "initial_equity": 100000.0, "final_equity": 110000.0, "return_rate": 0.1, "max_drawdown": 0.05, "sharpe_ratio": 1.5}` |
| GET | /api/backtests/{id}/equity | 获取权益曲线 | N/A | `{"id": "backtest_id", "equity_curve": [100000.0, 101000.0, 102000.0, ...]}` |
| GET | /api/backtests/{id}/status | 获取回测状态 | N/A | `{"id": "backtest_id", "status": "completed", "progress": 100}` |

### 5.4 系统管理接口

| 方法 | 路径 | 功能 | 请求体 | 成功响应 |
|------|------|------|--------|----------|
| GET | /api/system/status | 获取系统状态 | N/A | `{"status": "healthy", "version": "1.0.0", "uptime": 3600}` |
| GET | /api/system/metrics | 获取系统指标 | N/A | `{"cpu_usage": 25.5, "memory_usage": 512.0, "active_backtests": 2}` |

## 6. 数据库设计

### 6.1 数据表结构

#### `data_files`表
| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | `TEXT` | `PRIMARY KEY` | 数据文件ID |
| `symbol` | `TEXT` | `NOT NULL` | 股票代码 |
| `file_path` | `TEXT` | `NOT NULL` | 文件路径 |
| `start_date` | `DATETIME` | `NOT NULL` | 数据开始日期 |
| `end_date` | `DATETIME` | `NOT NULL` | 数据结束日期 |
| `record_count` | `INTEGER` | `NOT NULL` | 记录数量 |
| `upload_time` | `DATETIME` | `NOT NULL` | 上传时间 |

#### `strategies`表
| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | `TEXT` | `PRIMARY KEY` | 策略ID |
| `name` | `TEXT` | `NOT NULL` | 策略名称 |
| `type` | `TEXT` | `NOT NULL` | 策略类型 |
| `parameters` | `TEXT` | `NOT NULL` | 策略参数（JSON格式） |
| `created_at` | `DATETIME` | `NOT NULL` | 创建时间 |
| `updated_at` | `DATETIME` | `NOT NULL` | 更新时间 |

#### `backtests`表
| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | `TEXT` | `PRIMARY KEY` | 回测ID |
| `data_id` | `TEXT` | `REFERENCES data_files(id)` | 数据文件ID |
| `strategy_id` | `TEXT` | `REFERENCES strategies(id)` | 策略ID |
| `initial_equity` | `REAL` | `NOT NULL` | 初始资金 |
| `final_equity` | `REAL` | | 最终资金 |
| `return_rate` | `REAL` | | 收益率 |
| `max_drawdown` | `REAL` | | 最大回撤 |
| `sharpe_ratio` | `REAL` | | 夏普比率 |
| `status` | `TEXT` | `NOT NULL` | 状态（running/completed/failed） |
| `progress` | `INTEGER` | `NOT NULL` | 进度（0-100） |
| `start_time` | `DATETIME` | `NOT NULL` | 开始时间 |
| `end_time` | `DATETIME` | | 结束时间 |

#### `equity_curves`表
| 字段名 | 数据类型 | 约束 | 描述 |
|--------|----------|------|------|
| `id` | `INTEGER` | `PRIMARY KEY AUTOINCREMENT` | 记录ID |
| `backtest_id` | `TEXT` | `REFERENCES backtests(id)` | 回测ID |
| `timestamp` | `DATETIME` | `NOT NULL` | 时间戳 |
| `equity` | `REAL` | `NOT NULL` | 权益值 |

### 6.2 数据存储策略

1. **原始数据**：CSV文件存储在文件系统中，数据库中仅保存文件路径和元数据
2. **策略配置**：存储在数据库中，参数以JSON格式保存
3. **回测结果**：存储在数据库中，权益曲线单独存储以提高查询性能
4. **缓存策略**：对于频繁访问的数据，使用内存缓存减少数据库访问

## 7. 部署方案

### 7.1 依赖安装

- **Drogon**：通过vcpkg安装
- **SQLite**：系统自带或通过vcpkg安装
- **其他依赖**：nlohmann/json（JSON处理）

### 7.2 构建流程

1. 配置CMake
2. 编译项目
3. 生成可执行文件

### 7.3 运行环境

- **操作系统**：支持Linux、macOS、Windows
- **硬件要求**：根据数据量和并发需求调整
- **端口配置**：默认8080端口，可通过环境变量或配置文件修改

### 7.4 容器化

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
RUN /vcpkg/vcpkg install drogon nlohmann-json

WORKDIR /app

COPY . /app

# 构建项目
RUN mkdir -p build && cd build && \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/vcpkg/scripts/buildsystems/vcpkg.cmake && \
    cmake --build .

# 运行服务
CMD ["/app/build/stock_backtest_service"]
```

## 8. 性能优化策略

1. **异步处理**：使用Drogon的异步特性处理请求
2. **线程池**：利用现有线程池处理回测任务
3. **缓存**：对频繁访问的数据和计算结果进行缓存
4. **数据库优化**：合理设计索引，使用事务批量操作
5. **内存管理**：避免内存泄漏，合理使用智能指针

## 9. 安全考虑

1. **输入验证**：对所有API输入进行严格验证
2. **文件上传安全**：限制文件大小和类型，防止恶意文件上传
3. **SQL注入防护**：使用参数化查询
4. **错误处理**：不向客户端暴露详细错误信息
5. **日志记录**：记录关键操作和错误信息

## 10. 扩展性设计

1. **模块化**：各模块间低耦合，便于扩展
2. **插件系统**：支持自定义策略和指标
3. **配置管理**：使用配置文件管理系统参数
4. **监控系统**：集成监控和告警机制

## 11. 开发计划

1. **阶段1**：搭建基础框架，实现API服务层
2. **阶段2**：集成现有回测引擎，实现核心功能
3. **阶段3**：实现数据持久化和管理功能
4. **阶段4**：优化性能和安全性
5. **阶段5**：测试和部署

## 12. 总结

本架构设计提供了一个完整的后端服务方案，将现有的C++股票回测系统转变为一个可通过API访问的服务。采用Drogon框架和SQLite数据库，既保证了系统的高性能，又简化了部署和维护。通过合理的模块划分和API设计，系统具有良好的扩展性和可维护性，能够满足不同用户的需求。