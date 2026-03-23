# 项目进度文档

## 项目概览

本项目是一个高性能的多线程C++股票交易回测系统，提供完整的Web界面和REST API服务，支持多种策略的并行回测和性能评估。

## 当前进度

### 已实现的模块

#### 1. 类型定义模块 (`core/include/types.h`, `core/src/types.cpp`)
- 定义了基础数据类型：`Bar`（K线数据）、`Order`（订单）、`Position`（持仓）、`Portfolio`（投资组合）、`BacktestResult`（回测结果）
- 实现了投资组合价值计算方法 `get_total_value()`
- 支持时间戳和价格数据的存储和处理

#### 2. 线程池模块 (`core/include/thread_pool.h`, `core/src/thread_pool.cpp`)
- 实现了通用线程池，支持多线程任务调度
- 支持任务提交和执行，提高回测性能
- 包含完整的线程池测试文件 `test_thread_pool.cpp`

#### 3. 数据处理模块 (`core/include/data_handler.h`, `core/src/data_handler.cpp`)
- 实现了从CSV文件加载历史行情数据的功能
- 支持按时间范围查询数据
- 提供数据符号管理和时间范围查询接口
- 支持多股票数据管理

#### 4. 指标计算模块 (`core/include/indicator.h`, `core/src/indicator.cpp`)
- 实现了简单移动平均线 (SMA) 计算
- 实现了指数移动平均线 (EMA) 计算
- 提供了基于K线数据的指标计算接口

#### 5. 策略模块 (`core/include/strategy.h`, `core/src/strategy.cpp`)
- 定义了策略基类 `Strategy`，提供统一接口
- 实现了移动平均线交叉策略 (`MovingAverageCrossStrategy`)
- 支持自定义交易信号生成（买入、卖出、持有）
- 包含策略测试文件 `test_strategy.cpp`

#### 6. 执行模块 (`core/include/execution_handler.h`, `core/src/execution_handler.cpp`)
- 管理投资组合和现金余额
- 执行交易信号并更新持仓
- 提供投资组合状态查询接口
- 包含执行模块测试文件 `test_execution.cpp` 和 `test_execution_direct.cpp`

#### 7. 回测引擎 (`core/include/backtest_engine.h`, `core/src/backtest_engine.cpp`)
- 协调数据、策略和执行模块进行回测
- 实现了完整的回测流程：数据加载 → 信号生成 → 交易执行 → 绩效评估
- 计算绩效指标：总收益率、最大回撤、夏普比率、索提诺比率
- 生成权益曲线，支持回测结果分析
- 包含回测引擎测试文件 `test_backtest.cpp`

#### 8. 数据库模块 (`backend/models/Database.h`, `backend/models/Database.cpp`)
- 实现了SQLite数据库接口和完整的CRUD操作
- 支持数据文件、策略、回测结果的存储和管理
- 使用nlohmann/json库进行JSON数据处理
- 创建了完整的数据库表结构（data_files, strategies, backtests, equity_curves）

#### 9. 后端架构模块
- `backend/api/APIServer.h`：RESTful API服务器接口定义
- `backend/cli/CLI.h`：CLI命令行接口定义和实现，支持数据管理、策略管理、回测执行等命令
- `backend/services/BacktestService.h`：回测业务服务接口定义
- `backend/utils/Utils.h`：工具函数接口定义

#### 10. 主程序框架 (`core/src/main.cpp`)
- 搭建了系统的完整框架
- 集成了各个模块的调用
- 提供了用户交互界面

#### 11. 项目配置文件
- `CMakeLists.txt`：项目构建配置，支持分层架构和外部依赖
- `.gitignore`：Git忽略文件配置
- `README.md`：项目文档
- `backend_architecture.md`：后端架构设计文档
- `frontend_architecture.md`：前端架构设计文档
- `UIdesign.md`：UI/UX设计规范文档
- `PROGRESS.md`：项目进度文档

### 已实现的功能特性

1. **完整的回测流程**：从数据加载到绩效评估的端到端实现
2. **多策略支持**：基于策略基类的可扩展架构
3. **性能指标计算**：收益率、最大回撤、夏普比率等关键指标
4. **投资组合管理**：实时跟踪持仓和现金余额
5. **信号生成机制**：支持复杂的交易信号逻辑
6. **数据查询接口**：灵活的数据访问和时间范围查询
7. **多线程支持**：通过线程池提高回测性能
8. **完整的测试覆盖**：所有模块都有对应的测试文件

### 测试覆盖

项目包含完整的测试套件：

- `test_thread_pool.cpp`：线程池功能测试
- `test_strategy.cpp`：策略模块测试
- `test_indicator.cpp`：指标计算测试
- `test_execution.cpp`：执行模块测试
- `test_execution_direct.cpp`：执行模块直接测试
- `test_backtest.cpp`：回测引擎测试

### 架构文档

项目包含详细的架构设计文档：

- `backend_architecture.md`：后端架构设计文档，包含完整的API接口设计、数据库设计、部署方案等
- `frontend_architecture.md`：前端架构设计文档，包含React技术栈、组件设计、状态管理、API服务等
- `UIdesign.md`：UI/UX设计规范文档

### UI预览项目

项目包含完整的UI预览实现：

- `ui-preview/`：基于HTML5/CSS3/JavaScript的UI预览
- 包含完整的设计系统（颜色、排版、间距、阴影、圆角）
- 所有核心页面的HTML实现
- 响应式设计（移动端、平板、桌面端）
- 主题切换功能（浅色/深色模式）

### 前端项目框架

项目包含React前端项目框架：

- `frontend/`：基于React + Vite的前端项目
- 使用TypeScript进行类型检查
- 配置了ESLint和Prettier代码规范
- 包含基础的组件结构和路由配置

## 未实现的模块

### 后端模块

1. **API服务层** (Drogon框架集成)
   - REST API接口实现
   - 请求处理和响应
   - 认证授权机制
   - 错误处理和日志记录
   - WebSocket支持

2. **数据管理模块** (数据库集成)
   - 数据上传和存储
   - 数据检索和查询
   - 数据文件管理
   - 数据验证和预处理

3. **配置管理模块**
   - 配置文件加载和管理
   - 参数化配置支持
   - 环境变量支持

4. **日志模块**
   - 分级日志系统
   - 日志文件管理
   - 日志轮转和归档

### 前端模块

1. **数据管理界面**
   - CSV文件上传组件
   - 数据列表展示组件
   - 数据详情查看组件
   - 数据预览和验证组件

2. **策略管理界面**
   - 策略创建和配置组件
   - 策略列表展示组件
   - 策略详情查看组件
   - 参数编辑和模板选择组件

3. **回测执行界面**
   - 回测配置组件
   - 回测任务列表组件
   - 回测状态监控组件
   - 实时进度显示组件

4. **结果可视化界面**
   - 权益曲线图表组件
   - 绩效指标面板组件
   - 交易记录表格组件
   - 报告导出组件

5. **API服务集成**
   - API客户端封装
   - HTTP请求拦截器
   - 错误处理机制
   - WebSocket服务

6. **状态管理**
   - 数据状态管理
   - 策略状态管理
   - 回测状态管理
   - UI状态管理

## 技术栈

### 后端技术栈
- **语言**：C++20
- **Web框架**：Drogon（规划中）
- **数据库**：SQLite3
- **JSON处理**：nlohmann/json
- **构建工具**：CMake 3.16+
- **依赖管理**：vcpkg
- **版本控制**：Git

### 前端技术栈
- **框架**：React 18.x
- **状态管理**：Zustand
- **UI组件库**：Ant Design 5.x
- **图表库**：ECharts 5.x
- **构建工具**：Vite 5.x
- **路由**：React Router 6.x
- **HTTP客户端**：Axios 1.x
- **类型检查**：TypeScript 5.x
- **代码规范**：ESLint + Prettier

## 项目结构

```
stock_backtrack/
├── core/                     # 核心回测引擎
│   ├── include/              # 核心头文件
│   │   ├── backtest_engine.h    # 回测引擎
│   │   ├── data_handler.h        # 数据处理
│   │   ├── execution_handler.h   # 执行处理
│   │   ├── indicator.h           # 指标计算
│   │   ├── strategy.h            # 策略模块
│   │   ├── thread_pool.h         # 线程池
│   │   └── types.h               # 类型定义
│   └── src/                  # 核心源代码
│       ├── backtest_engine.cpp
│       ├── data_handler.cpp
│       ├── execution_handler.cpp
│       ├── indicator.cpp
│       ├── main.cpp
│       ├── strategy.cpp
│       ├── thread_pool.cpp
│       └── types.cpp
├── backend/                  # 后端服务层
│   ├── api/                  # API服务接口
│   │   └── APIServer.h
│   ├── cli/                  # CLI命令行接口
│   │   └── CLI.h
│   ├── models/               # 数据模型
│   │   ├── Database.h
│   │   └── Database.cpp
│   ├── services/             # 业务服务
│   │   └── BacktestService.h
│   └── utils/                # 工具函数
│       └── Utils.h
├── frontend/                 # 前端项目
│   ├── public/              # 公共资源
│   ├── src/                 # 源代码
│   │   ├── assets/          # 静态资源
│   │   ├── stores/          # 状态管理
│   │   ├── types/           # 类型定义
│   │   ├── App.tsx          # 主应用组件
│   │   └── main.tsx         # 入口文件
│   ├── package.json
│   └── vite.config.ts
├── test/                     # 测试文件
│   ├── test_backtest.cpp
│   ├── test_execution.cpp
│   ├── test_execution_direct.cpp
│   ├── test_indicator.cpp
│   ├── test_strategy.cpp
│   └── test_thread_pool.cpp
├── ui-preview/               # UI预览
│   ├── css/                  # 样式文件
│   ├── js/                   # JavaScript文件
│   └── index.html
├── data/                     # 数据目录
├── .gitignore                # Git忽略文件
├── CMakeLists.txt            # CMake配置
├── backend_architecture.md   # 后端架构文档
├── frontend_architecture.md  # 前端架构文档
├── UIdesign.md              # UI设计文档
├── README.md                 # 项目文档
└── PROGRESS.md               # 项目进度文档
```

## 下一步计划

### 短期计划（1-2周）

#### 后端开发
1. **安装和配置外部依赖**
   - 通过vcpkg安装Drogon、nlohmann/json、CLI11
   - 配置CMake构建系统
   - 确保所有依赖正确集成

2. **实现API服务层**
   - 基于Drogon框架实现REST API接口
   - 实现数据管理API接口（上传、查询、删除）
   - 实现策略管理API接口（创建、更新、删除）
   - 实现回测管理API接口（运行、查询结果）
   - 实现系统管理API接口（状态、指标）

3. **完善数据库功能**
   - 实现数据文件上传和存储
   - 实现策略参数的JSON序列化和反序列化
   - 实现回测结果的持久化存储
   - 优化数据库查询性能

#### 前端开发
1. **搭建前端项目结构**
   - 完善项目目录结构
   - 配置路由和状态管理
   - 集成Ant Design组件库

2. **实现数据管理界面**
   - 实现文件上传组件
   - 实现数据列表组件
   - 实现数据详情组件

### 中期计划（2-4周）

#### 后端开发
1. **完善API功能**
   - 实现WebSocket支持
   - 完善错误处理机制
   - 实现请求验证和安全防护

2. **性能优化**
   - 优化数据库查询
   - 实现缓存机制
   - 优化多线程处理

3. **配置管理和日志**
   - 实现配置管理模块
   - 实现日志记录系统
   - 完善错误处理和监控

#### 前端开发
1. **实现策略管理界面**
   - 实现策略配置组件
   - 实现策略列表组件
   - 实现参数编辑组件

2. **实现回测执行界面**
   - 实现回测配置组件
   - 实现任务列表组件
   - 实现进度监控组件

3. **实现结果可视化界面**
   - 实现权益曲线图表
   - 实现绩效指标面板
   - 实现交易记录表格

### 长期计划（1-2个月）

1. **系统优化**
   - 性能优化和调优
   - 安全性增强
   - 稳定性提升

2. **功能扩展**
   - 增加更多策略示例
   - 支持更多数据格式
   - 实现报告导出功能

3. **测试和部署**
   - 编写完整的测试套件
   - 实现自动化测试
   - 容器化部署
   - CI/CD流程

4. **文档完善**
   - API文档完善
   - 用户手册编写
   - 开发者文档更新

## 贡献指南

1. 克隆仓库：`git clone https://gitee.com/zhang-xuanhua123/open-stock-backtrack.git`
2. 创建分支：`git checkout -b feature/<功能名称>`
3. 提交代码：`git commit -m "Add: <功能描述>"`
4. 推送到远程：`git push origin feature/<功能名称>`
5. 创建Pull Request

## 问题与解决方案

### 已解决的问题

1. **项目架构重构**：成功将项目重构为core/backend/frontend三层架构，提高了代码组织清晰度
2. **数据库模块实现**：实现了完整的SQLite数据库接口，支持数据文件、策略、回测结果的存储和管理
3. **JSON库集成**：成功集成nlohmann/json库，替代了自定义的SimpleJSON类，提高了JSON处理能力
4. **CMake构建系统**：更新了CMakeLists.txt，支持分层架构和外部依赖管理
5. **多线程支持**：实现了线程池，支持并行回测提高性能
6. **策略框架**：设计了灵活的策略基类，支持自定义策略扩展
7. **绩效评估**：实现了完整的绩效指标计算，包括收益率、最大回撤、夏普比率等
8. **数据处理**：实现了CSV数据加载和时间范围查询功能
9. **测试覆盖**：完成了所有模块的单元测试，确保系统稳定性
10. **架构设计**：完成了详细的前后端架构设计文档
11. **UI设计**：完成了完整的UI预览和设计系统
12. **后端接口定义**：创建了API服务、CLI命令行、业务服务等接口定义

### 当前面临的挑战

1. **API服务层**：需要集成Drogon框架，实现REST API接口
2. **数据库集成**：需要完善数据库模块，实现数据持久化
3. **前端开发**：需要实现完整的React前端界面
4. **配置管理**：需要实现灵活的配置系统
5. **日志系统**：需要实现完善的日志记录功能
6. **WebSocket支持**：需要实现实时数据推送功能

### 解决方案

1. **API服务层**：使用Drogon框架，按照架构文档实现API接口
2. **数据库集成**：完善数据库模块，实现数据持久化和查询
3. **前端开发**：基于React + Ant Design，按照前端架构文档实现界面
4. **配置管理**：设计基于JSON或YAML的配置文件格式
5. **日志系统**：实现分级日志系统，支持不同级别的日志输出
6. **WebSocket支持**：使用Drogon的WebSocket功能，实现实时数据推送

## 总结

本项目已成功实现了一个完整的C++股票交易回测系统的核心功能，包括以下方面：

### 后端核心功能
1. **完整的回测流程**：从数据加载、策略信号生成、交易执行到绩效评估的端到端实现
2. **模块化设计**：采用清晰的分层架构，包括数据处理、策略、执行、回测引擎等独立模块
3. **多线程支持**：通过线程池实现并行回测，提高系统性能
4. **灵活的策略框架**：支持自定义策略扩展，已实现移动平均线交叉策略
5. **完善的绩效评估**：计算收益率、最大回撤、夏普比率等关键绩效指标
6. **完整的测试覆盖**：所有模块都有对应的测试文件，确保系统稳定性

### 架构设计
1. **详细的后端架构设计**：包含完整的API接口设计、数据库设计、部署方案等
2. **详细的前端架构设计**：包含React技术栈、组件设计、状态管理、API服务等
3. **完整的UI设计系统**：包含设计规范、组件库、响应式设计等
4. **UI预览实现**：基于HTML5/CSS3/JavaScript的完整UI预览

### 前端基础
1. **React项目框架**：基于React + Vite的前端项目已搭建
2. **开发环境配置**：TypeScript、ESLint、Prettier等已配置完成
3. **基础结构**：组件、页面、服务等目录结构已建立

项目采用现代C++20标准开发，具有良好的可扩展性和性能。核心功能已全部实现，系统可以进行完整的股票交易回测。下一步将重点实现API服务层、前端界面开发、数据库集成等功能，进一步完善系统的实用性和易用性。

## 联系方式

如有问题或建议，欢迎通过以下方式联系：
- Email: 13564701211@163.com
- Gitee: https://gitee.com/zhang-xuanhua123/open-stock-backtrack