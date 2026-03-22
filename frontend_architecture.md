# 股票回测系统前端架构设计

## 📋 目录索引

- [1. 架构概述](#1-架构概述)
- [2. 技术栈选择](#2-技术栈选择)
- [3. 系统架构图](#3-系统架构图)
- [4. 目录结构设计](#4-目录结构设计)
- [5. 核心模块设计](#5-核心模块设计)
- [6. 路由设计](#6-路由设计)
- [7. 状态管理设计](#7-状态管理设计)
- [8. API服务设计](#8-api服务设计)
- [9. UI/UX设计](#9-uiux设计)
- [10. 性能优化策略](#10-性能优化策略)
- [11. 开发指南](#11-开发指南)
- [12. 参考资源](#12-参考资源)

## 1. 架构概述

本架构设计为股票回测系统提供现代化、高性能的Web前端界面。采用React技术栈，提供完整的回测功能交互界面，包括数据管理、策略配置、回测执行、结果可视化等核心功能。

**核心特性：**
- 组件化架构，易于维护和扩展
- 响应式设计，支持多设备访问
- 实时数据更新，WebSocket支持
- 完整的状态管理和错误处理
- 优秀的用户体验和性能表现

## 2. 技术栈选择

| 组件 | 技术 | 版本 | 选型理由 |
|------|------|------|----------|
| 前端框架 | React | 18.x | 组件化开发，生态成熟，性能优秀 |
| 状态管理 | Zustand | 最新版 | 轻量级，API简洁，TypeScript支持好 |
| UI组件库 | Ant Design | 5.x | 企业级组件库，功能完善，中文文档 |
| 图表库 | ECharts | 5.x | 功能强大，性能优秀，支持金融图表 |
| 构建工具 | Vite | 5.x | 快速开发体验，现代化构建工具 |
| 路由 | React Router | 6.x | 官方推荐，支持懒加载 |
| HTTP客户端 | Axios | 1.x | 拦截器完善，请求取消支持 |
| 类型检查 | TypeScript | 5.x | 类型安全，提升开发体验 |
| 代码规范 | ESLint + Prettier | 最新版 | 代码质量保证 |
| 测试框架 | Vitest + Testing Library | 最新版 | 单元测试和组件测试 |

## 3. 系统架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                         前端应用层                                │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │  数据管理   │  │  策略管理   │  │  回测管理   │              │
│  └─────────────┘  └─────────────┘  └─────────────┘              │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │  结果可视化 │  │  系统监控   │  │  用户设置   │              │
│  └─────────────┘  └─────────────┘  └─────────────┘              │
├─────────────────────────────────────────────────────────────────┤
│                        共享服务层                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │  API服务    │  │  状态管理   │  │  工具函数   │              │
│  └─────────────┘  └─────────────┘  └─────────────┘              │
├─────────────────────────────────────────────────────────────────┤
│                        基础设施层                                 │
├─────────────────────────────────────────────────────────────────┤
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐              │
│  │  路由管理   │  │  样式主题   │  │  国际化     │              │
│  └─────────────┘  └─────────────┘  └─────────────┘              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              │ HTTP/HTTPS + WebSocket
                              │
┌─────────────────────────────────────────────────────────────────┐
│                      后端API服务                                 │
└─────────────────────────────────────────────────────────────────┘
```

## 4. 目录结构设计

```
frontend/
├── src/
│   ├── components/             # 通用组件
│   │   ├── common/            # 基础组件 (Button, Card, Modal等)
│   │   ├── layout/            # 布局组件 (Header, Sidebar等)
│   │   └── charts/            # 图表组件 (EquityCurve等)
│   ├── pages/                 # 页面组件
│   │   ├── Dashboard/         # 仪表盘
│   │   ├── DataManagement/    # 数据管理
│   │   ├── StrategyManagement/# 策略管理
│   │   ├── Backtest/          # 回测执行
│   │   ├── Results/           # 结果展示
│   │   └── Settings/          # 系统设置
│   ├── services/              # API服务
│   │   ├── api/               # API客户端
│   │   ├── http/              # HTTP配置
│   │   └── websocket/         # WebSocket服务
│   ├── stores/                # 状态管理 (Zustand)
│   ├── hooks/                 # 自定义Hooks
│   ├── utils/                 # 工具函数
│   ├── types/                 # TypeScript类型定义
│   ├── router/                # 路由配置
│   ├── theme/                 # 主题配置
│   ├── App.tsx                # 根组件
│   └── main.tsx               # 入口文件
├── public/                    # 公共资源
├── tests/                     # 测试文件
└── 配置文件 (vite.config.ts, tsconfig.json等)
```

## 5. 核心模块设计

### 5.1 数据管理模块

**核心组件：**
- `DataUploadForm` - 文件上传表单
- `DataListTable` - 数据列表表格
- `DataDetailModal` - 数据详情弹窗
- `DataPreviewChart` - 数据预览图表

**状态管理：**
- `dataList` - 数据文件列表
- `currentData` - 当前选中的数据
- `uploadProgress` - 上传进度

### 5.2 策略管理模块

**核心组件：**
- `StrategyForm` - 策略配置表单
- `StrategyList` - 策略列表
- `StrategyParameterEditor` - 参数编辑器
- `StrategyTemplateSelector` - 模板选择器

**状态管理：**
- `strategyList` - 策略列表
- `currentStrategy` - 当前策略
- `strategyTemplates` - 策略模板

### 5.3 回测管理模块

**核心组件：**
- `BacktestConfigForm` - 回测配置表单
- `BacktestList` - 回测任务列表
- `BacktestStatusCard` - 状态卡片
- `BacktestProgress` - 进度条

**状态管理：**
- `backtestList` - 回测任务列表
- `runningBacktests` - 运行中的回测
- `backtestProgress` - 回测进度

### 5.4 结果可视化模块

**核心组件：**
- `EquityCurveChart` - 权益曲线图
- `PerformanceMetrics` - 绩效指标面板
- `TradeHistoryTable` - 交易记录表
- `SignalChart` - 信号图表
- `ReportExporter` - 报告导出器

**状态管理：**
- `backtestResults` - 回测结果
- `equityCurve` - 权益曲线数据
- `tradeHistory` - 交易记录

## 6. 路由设计

### 6.1 路由结构

```
/                           # 首页/仪表盘
├── /dashboard              # 仪表盘
├── /data                   # 数据管理
│   ├── /data/upload        # 上传数据
│   └── /data/:id           # 数据详情
├── /strategies             # 策略管理
│   ├── /strategies/create  # 创建策略
│   └── /strategies/:id     # 策略详情
├── /backtests              # 回测管理
│   ├── /backtests/new      # 新建回测
│   ├── /backtests/:id      # 回测详情
│   └── /backtests/:id/results # 回测结果
├── /results                # 结果分析
│   └── /results/:id        # 结果详情
└── /settings               # 系统设置
```

### 6.2 路由守卫

- 认证守卫（如果需要）
- 权限守卫
- 数据加载守卫
- 导航确认守卫

## 7. 状态管理设计

### 7.1 状态分层

```
全局状态 (Global) → 业务状态 (Business) → UI状态 (UI)
```

### 7.2 Store设计

使用 Zustand 实现轻量级状态管理：

**dataStore.ts**
- 数据列表
- 当前选中数据
- 上传状态
- 过滤条件

**strategyStore.ts**
- 策略列表
- 当前策略
- 策略模板
- 编辑状态

**backtestStore.ts**
- 回测任务列表
- 运行中的回测
- 回测结果
- 进度信息

**uiStore.ts**
- 加载状态
- 错误信息
- 通知消息
- 模态框状态

## 8. API服务设计

### 8.1 API客户端封装

**axios配置：**
- 基础URL配置
- 请求/响应拦截器
- 错误处理
- 请求取消
- 超时控制

### 8.2 API服务模块

**data.ts**
- `uploadData(file)` - 上传数据
- `getDataList()` - 获取数据列表
- `getDataDetail(id)` - 获取数据详情
- `deleteData(id)` - 删除数据

**strategy.ts**
- `createStrategy(data)` - 创建策略
- `getStrategyList()` - 获取策略列表
- `getStrategyDetail(id)` - 获取策略详情
- `updateStrategy(id, data)` - 更新策略
- `deleteStrategy(id)` - 删除策略

**backtest.ts**
- `runBacktest(data)` - 运行回测
- `getBacktestList()` - 获取回测列表
- `getBacktestDetail(id)` - 获取回测详情
- `getBacktestStatus(id)` - 获取回测状态
- `getEquityCurve(id)` - 获取权益曲线
- `cancelBacktest(id)` - 取消回测

**system.ts**
- `getSystemStatus()` - 获取系统状态
- `getSystemMetrics()` - 获取系统指标

### 8.3 WebSocket服务

**事件类型：**
- `backtest:progress` - 回测进度
- `backtest:completed` - 回测完成
- `backtest:failed` - 回测失败
- `system:status` - 系统状态
- `system:error` - 系统错误

## 9. UI/UX设计

### 9.1 设计原则

- **一致性**：统一的视觉风格和交互模式
- **响应式**：适配不同屏幕尺寸
- **可访问性**：遵循WCAG 2.1标准
- **性能**：快速响应，流畅动画
- **易用性**：直观的操作流程

### 9.2 主题系统

**颜色方案：**
- 主色调：`#1890ff`（蓝色系）
- 成功色：`#52c41a`（绿色）
- 警告色：`#faad14`（橙色）
- 错误色：`#f5222d`（红色）
- 中性色：灰色系

**主题模式：**
- 浅色模式（默认）
- 深色模式
- 自动切换（跟随系统）

### 9.3 布局设计

**主布局：**
```
┌─────────────────────────────────────────┐
│              Header (顶部导航)            │
├──────────┬──────────────────────────────┤
│          │                              │
│ Sidebar  │      Main Content Area       │
│ (侧边栏) │      (主内容区域)            │
│          │                              │
└──────────┴──────────────────────────────┘
```

**响应式断点：**
- 移动端：< 768px
- 平板：768px - 1024px
- 桌面：> 1024px

## 10. 性能优化策略

### 10.1 代码优化

- **代码分割**：路由级别的懒加载
- **Tree Shaking**：移除未使用的代码
- **压缩优化**：代码和资源压缩
- **缓存策略**：浏览器缓存和HTTP缓存

### 10.2 渲染优化

- **虚拟滚动**：长列表优化
- **防抖节流**：频繁操作优化
- **Memo优化**：组件记忆化
- **懒加载**：图片和组件懒加载

### 10.3 数据优化

- **数据缓存**：API响应缓存
- **分页加载**：大数据分页
- **增量更新**：WebSocket增量推送
- **数据预取**：预测用户行为

## 11. 开发指南

### 11.1 快速开始

```bash
# 安装依赖
npm install

# 启动开发服务器
npm run dev

# 构建生产版本
npm run build

# 运行测试
npm test
```

### 11.2 代码规范

- 使用 TypeScript 进行类型检查
- 遵循 ESLint 和 Prettier 配置
- 组件使用函数式组件 + Hooks
- 使用语义化 HTML 标签
- 遵循 React 最佳实践

### 11.3 组件开发规范

```typescript
// 组件模板
import React from 'react';

interface ComponentProps {
  // 定义props类型
}

export const Component: React.FC<ComponentProps> = ({ props }) => {
  // 组件逻辑
  return (
    <div>
      {/* JSX */}
    </div>
  );
};

export default Component;
```

### 11.4 状态管理规范

```typescript
// Store模板
import { create } from 'zustand';

interface StoreState {
  // 状态定义
  state: any;
  // 操作方法
  actions: () => void;
}

export const useStore = create<StoreState>((set) => ({
  state: initialState,
  actions: () => set({ state: newValue }),
}));
```

## 12. 参考资源

### 12.1 设计参考

**UI预览项目：**
- 📁 路径：`ui-preview/`
- 📄 说明：`ui-preview/README.md`
- 🎨 功能：完整的HTML5/CSS3/JavaScript UI预览

**UI预览包含：**
- 完整的设计系统（颜色、排版、间距、阴影、圆角）
- 所有核心页面的HTML实现
- 响应式设计（移动端、平板、桌面端）
- 主题切换功能（浅色/深色模式）
- 丰富的组件库和交互功能
- 实际可运行的预览界面

**使用建议：**
1. **样式参考**：参考 `ui-preview/css/` 中的CSS变量和样式定义
2. **布局参考**：参考 `ui-preview/index.html` 中的页面布局结构
3. **交互参考**：参考 `ui-preview/js/app.js` 中的交互逻辑
4. **组件参考**：参考 `ui-preview/css/components.css` 中的组件样式
5. **响应式参考**：参考 `ui-preview/css/responsive.css` 中的响应式断点

### 12.2 技术文档

- [React 官方文档](https://react.dev/)
- [TypeScript 官方文档](https://www.typescriptlang.org/)
- [Ant Design 组件库](https://ant.design/)
- [ECharts 图表库](https://echarts.apache.org/)
- [Zustand 状态管理](https://github.com/pmndrs/zustand)
- [Vite 构建工具](https://vitejs.dev/)
- [React Router 路由](https://reactrouter.com/)

### 12.3 最佳实践

- **React 最佳实践**：[React 官方最佳实践](https://react.dev/learn/thinking-in-react)
- **TypeScript 最佳实践**：[TypeScript 官方指南](https://www.typescriptlang.org/docs/handbook/declaration-files/do-s-and-don-ts.html)
- **性能优化**：[React 性能优化](https://react.dev/learn/render-and-commit)
- **可访问性**：[WCAG 2.1 指南](https://www.w3.org/WAI/WCAG21/quickref/)

### 12.4 项目相关文档

- **后端架构**：`backend_architecture.md`
- **UI设计**：`UIdesign.md`
- **项目进度**：`PROGRESS.md`
- **C++开发规则**：`.trae/rules/aitutor.md`

### 12.5 开发工作流

1. **需求分析**：阅读相关架构文档和UI设计文档
2. **UI参考**：查看 `ui-preview/` 中的实现效果
3. **组件开发**：基于Ant Design组件库开发自定义组件
4. **样式实现**：参考 `ui-preview/css/` 中的样式规范
5. **状态管理**：使用Zustand进行状态管理
6. **API集成**：按照API服务设计进行接口对接
7. **测试验证**：编写单元测试和集成测试
8. **性能优化**：应用性能优化策略

## 附录

### A. 设计系统规范

详细的设计系统规范请参考 `UIdesign.md` 文档，包括：
- 完整的颜色方案和主题系统
- 详细的排版系统和字体规范
- 间距系统和布局规范
- 阴影和圆角系统
- 组件设计规范
- 交互设计规范

### B. UI预览使用指南

详细的UI预览使用指南请参考 `ui-preview/README.md` 文档，包括：
- 功能特性说明
- 使用方法和启动方式
- 功能演示指南
- 设计规范说明
- 自定义和扩展方法

### C. 常见问题

**Q: 如何开始开发？**
A: 先阅读本文档了解架构，然后查看 `ui-preview/` 了解UI效果，最后参考开发指南开始编码。

**Q: 样式如何实现？**
A: 使用Ant Design作为基础组件库，参考 `ui-preview/css/` 中的样式规范进行自定义样式开发。

**Q: 状态管理如何选择？**
A: 推荐使用Zustand，它轻量级、API简洁，TypeScript支持好。

**Q: 如何保证代码质量？**
A: 遵循ESLint和Prettier配置，编写TypeScript类型定义，编写单元测试。

**D: 如何进行性能优化？**
A: 参考本文档第10节"性能优化策略"，应用代码分割、懒加载、缓存等优化技术。

---

**文档版本**：v2.0
**最后更新**：2026-03-22
**维护者**：开发团队