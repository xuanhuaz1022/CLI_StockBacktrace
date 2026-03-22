# 股票交易回测系统

一个高性能的多线程C++股票交易回测系统，提供完整的Web界面和REST API服务，支持多种策略的并行回测和性能评估。

## 系统架构

### 整体架构

本系统采用前后端分离架构，提供完整的Web界面和REST API服务：

```
┌─────────────────────────────────────────────────────────────────┐
│                         前端应用 (React)                           │
│  - 数据管理界面  - 策略配置界面  - 回测执行界面  - 结果可视化      │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ HTTP/HTTPS + WebSocket
                              │
┌─────────────────────────────────────────────────────────────────┐
│                    后端API服务 (Drogon + C++)                     │
│  - REST API  - 数据管理  - 策略管理  - 回测引擎  - 绩效评估        │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │
┌─────────────────────────────────────────────────────────────────┐
│                    核心回测引擎 (C++20)                            │
│  - 数据处理  - 策略执行  - 交易模拟  - 多线程优化                  │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │
┌─────────────────────────────────────────────────────────────────┐
│                    数据存储 (SQLite + 文件系统)                   │
│  - 历史行情数据  - 策略配置  - 回测结果  - 权益曲线               │
└─────────────────────────────────────────────────────────────────┘
```

### 技术栈

#### 后端技术栈
| 组件 | 技术 | 版本 | 说明 |
|------|------|------|------|
| 后端语言 | C++ | C++20 | 高性能，适合计算密集型任务 |
| Web框架 | Drogon | 最新稳定版 | 高性能异步框架，支持现代C++ |
| 数据库 | SQLite | 3.x | 轻量级，无需独立服务器 |
| 构建工具 | CMake | 3.16+ | 跨平台构建工具 |
| 依赖管理 | vcpkg | 最新版 | 第三方库依赖管理 |

#### 前端技术栈
| 组件 | 技术 | 版本 | 说明 |
|------|------|------|------|
| 前端框架 | React | 18.x | 组件化开发，生态成熟 |
| 状态管理 | Zustand | 最新版 | 轻量级状态管理 |
| UI组件库 | Ant Design | 5.x | 企业级组件库 |
| 图表库 | ECharts | 5.x | 功能强大的金融图表库 |
| 构建工具 | Vite | 5.x | 快速开发体验 |
| 路由 | React Router | 6.x | 官方推荐路由方案 |
| HTTP客户端 | Axios | 1.x | 完善的HTTP客户端 |
| 类型检查 | TypeScript | 5.x | 类型安全 |

### 核心模块

#### 后端模块

1. **数据处理模块** (`include/data_handler.h`, `src/data_handler.cpp`)
   - 从CSV文件加载历史行情数据
   - 数据预处理和验证
   - 时间范围查询和数据访问接口
   - 多股票数据管理

2. **策略模块** (`include/strategy.h`, `src/strategy.cpp`)
   - 策略基类定义统一接口
   - 移动平均线交叉策略实现
   - 支持自定义交易策略扩展
   - 交易信号生成机制

3. **回测引擎** (`include/backtest_engine.h`, `src/backtest_engine.cpp`)
   - 协调数据、策略和执行模块
   - 完整的回测流程管理
   - 多线程并行回测支持
   - 回测结果和绩效指标计算

4. **执行处理模块** (`include/execution_handler.h`, `src/execution_handler.cpp`)
   - 投资组合和现金余额管理
   - 交易信号执行和持仓更新
   - 投资组合状态查询接口

5. **指标计算模块** (`include/indicator.h`, `src/indicator.cpp`)
   - 简单移动平均线 (SMA) 计算
   - 指数移动平均线 (EMA) 计算
   - 基于K线数据的指标计算接口

6. **线程池** (`include/thread_pool.h`, `src/thread_pool.cpp`)
   - 多线程任务调度
   - 并行回测性能优化
   - 高效的任务队列管理

7. **API服务层** (规划中)
   - REST API接口实现
   - 请求处理和响应
   - 认证授权机制
   - 错误处理和日志记录

8. **数据管理模块** (规划中)
   - 数据上传和存储
   - 数据检索和查询
   - 数据文件管理

#### 前端模块

1. **数据管理界面**
   - CSV文件上传功能
   - 数据列表展示和详情查看
   - 数据预览和验证

2. **策略管理界面**
   - 策略创建和配置
   - 策略列表和详情查看
   - 参数编辑和模板选择

3. **回测执行界面**
   - 回测配置和启动
   - 回测任务列表和状态监控
   - 实时进度显示

4. **结果可视化界面**
   - 权益曲线图表展示
   - 绩效指标面板
   - 交易记录表格
   - 报告导出功能

## 系统特性

### 后端特性
- **高性能**：C++20 + 多线程并行处理，回测速度快
- **可扩展**：模块化设计，支持自定义策略和指标
- **完整的性能评估**：收益率、最大回撤、夏普比率、索提诺比率等
- **REST API**：提供完整的API接口，支持前端集成
- **数据持久化**：SQLite数据库存储，支持数据管理和查询
- **线程池优化**：高效的多线程任务调度

### 前端特性
- **现代化界面**：React + Ant Design，用户体验优秀
- **实时更新**：WebSocket支持，实时显示回测进度
- **响应式设计**：支持多设备访问
- **可视化图表**：ECharts金融图表，数据展示直观
- **状态管理**：Zustand轻量级状态管理，性能优秀
- **类型安全**：TypeScript类型检查，提升代码质量

## 项目结构

```
stock_backtrack/
├── include/                  # 后端头文件
│   ├── backtest_engine.h    # 回测引擎
│   ├── data_handler.h        # 数据处理
│   ├── execution_handler.h   # 执行处理
│   ├── indicator.h           # 指标计算
│   ├── strategy.h            # 策略模块
│   ├── thread_pool.h         # 线程池
│   └── types.h               # 类型定义
├── src/                      # 后端源代码
│   ├── backtest_engine.cpp
│   ├── data_handler.cpp
│   ├── execution_handler.cpp
│   ├── indicator.cpp
│   ├── main.cpp
│   ├── strategy.cpp
│   ├── thread_pool.cpp
│   └── types.cpp
├── test/                     # 测试文件
│   ├── test_backtest.cpp
│   ├── test_execution.cpp
│   ├── test_indicator.cpp
│   ├── test_strategy.cpp
│   └── test_thread_pool.cpp
├── frontend/                 # 前端项目
│   ├── src/
│   │   ├── components/      # 通用组件
│   │   ├── pages/           # 页面组件
│   │   ├── services/        # API服务
│   │   ├── stores/          # 状态管理
│   │   └── utils/           # 工具函数
│   └── package.json
├── ui-preview/               # UI预览
│   ├── css/                  # 样式文件
│   ├── js/                   # JavaScript文件
│   └── index.html
├── data/                     # 数据目录
├── CMakeLists.txt            # CMake配置
├── backend_architecture.md   # 后端架构文档
├── frontend_architecture.md  # 前端架构文档
├── README.md                 # 项目文档
└── PROGRESS.md               # 项目进度
```

## 快速开始

### 后端构建与运行

1. **安装依赖**
   ```bash
   # 安装CMake和C++20编译器
   # macOS
   brew install cmake

   # Ubuntu/Debian
   sudo apt-get install cmake build-essential
   ```

2. **构建项目**
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```

3. **运行回测**
   ```bash
   ./stock_backtrack
   ```

### 前端开发

1. **安装依赖**
   ```bash
   cd frontend
   npm install
   ```

2. **启动开发服务器**
   ```bash
   npm run dev
   ```

3. **构建生产版本**
   ```bash
   npm run build
   ```

## API接口文档

### 数据管理接口

| 方法 | 路径 | 功能 |
|------