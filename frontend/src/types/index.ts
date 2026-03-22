// 数据相关类型
export interface DataFile {
  id: string;
  filename: string;
  symbol: string;
  uploadTime: string;
  size: number;
  status: 'uploading' | 'completed' | 'failed';
  recordCount: number;
}

export interface DataInfo {
  id: string;
  symbol: string;
  name: string;
  startDate: string;
  endDate: string;
  recordCount: number;
}

// 策略相关类型
export interface StrategyParameter {
  name: string;
  value: number | string | boolean;
  type: 'number' | 'string' | 'boolean';
  description?: string;
  min?: number;
  max?: number;
}

export interface Strategy {
  id: string;
  name: string;
  type: 'moving_average' | 'momentum' | 'mean_reversion';
  parameters: StrategyParameter[];
  description?: string;
  createdAt: string;
  updatedAt: string;
}

// 回测相关类型
export interface BacktestConfig {
  dataId: string;
  strategyId: string;
  initialCapital: number;
  startDate: string;
  endDate: string;
  commissionRate: number;
  slippage: number;
}

export interface BacktestTask {
  id: string;
  config: BacktestConfig;
  status: 'pending' | 'running' | 'completed' | 'failed';
  progress: number;
  startTime?: string;
  endTime?: string;
  errorMessage?: string;
}

export interface BacktestResult {
  id: string;
  taskId: string;
  totalReturn: number;
  annualReturn: number;
  maxDrawdown: number;
  sharpeRatio: number;
  winRate: number;
  totalTrades: number;
  equityCurve: Array<{ date: string; value: number }>;
  tradeHistory: Array<{
    date: string;
    type: 'buy' | 'sell';
    price: number;
    quantity: number;
    amount: number;
  }>;
}

// UI相关类型
export interface LoadingState {
  isLoading: boolean;
  message?: string;
}

export interface Notification {
  id: string;
  type: 'success' | 'info' | 'warning' | 'error';
  message: string;
  description?: string;
  duration?: number;
}

export interface MenuItem {
  key: string;
  label: string;
  icon?: string;
  path?: string;
  children?: MenuItem[];
}

// API响应类型
export interface ApiResponse<T> {
  success: boolean;
  data?: T;
  error?: string;
}

// WebSocket事件类型
export type WebSocketEventType = 
  | 'backtest:progress'
  | 'backtest:completed'
  | 'backtest:failed'
  | 'system:status'
  | 'system:error';

export interface WebSocketEvent {
  type: WebSocketEventType;
  payload: any;
}