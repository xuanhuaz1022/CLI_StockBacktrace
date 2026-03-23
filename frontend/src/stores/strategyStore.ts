import { create } from 'zustand';
import type { Strategy, StrategyParameter } from '../types';

type StrategyTemplate = {
  id: string;
  name: string;
  type: string;
  parameters: StrategyParameter[];
  description: string;
};

interface StrategyStoreState {
  // 状态
  strategies: Strategy[];
  currentStrategy: Strategy | null;
  strategyTemplates: StrategyTemplate[];
  isLoading: boolean;
  error: string | null;
  
  // 操作方法
  setStrategies: (strategies: Strategy[]) => void;
  addStrategy: (strategy: Strategy) => void;
  updateStrategy: (id: string, updates: Partial<Strategy>) => void;
  deleteStrategy: (id: string) => void;
  setCurrentStrategy: (strategy: Strategy | null) => void;
  setStrategyTemplates: (templates: StrategyTemplate[]) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
}

export const useStrategyStore = create<StrategyStoreState>((set) => ({
  // 初始状态
  strategies: [],
  currentStrategy: null,
  strategyTemplates: [
    {
      id: 'ma_cross',
      name: '均线交叉策略',
      type: 'moving_average',
      parameters: [
        { name: 'shortPeriod', value: 5, type: 'number', description: '短期均线周期', min: 1, max: 100 },
        { name: 'longPeriod', value: 20, type: 'number', description: '长期均线周期', min: 1, max: 200 }
      ],
      description: '当短期均线上穿长期均线时买入，下穿时卖出'
    },
    {
      id: 'momentum',
      name: '动量策略',
      type: 'momentum',
      parameters: [
        { name: 'period', value: 10, type: 'number', description: '动量计算周期', min: 1, max: 60 },
        { name: 'threshold', value: 0.05, type: 'number', description: '动量阈值', min: 0, max: 1 }
      ],
      description: '基于价格动量变化进行交易'
    }
  ],
  isLoading: false,
  error: null,
  
  // 操作方法
  setStrategies: (strategies) => set({ strategies }),
  
  addStrategy: (strategy) => set((state) => ({
    strategies: [...state.strategies, strategy]
  })),
  
  updateStrategy: (id, updates) => set((state) => ({
    strategies: state.strategies.map(strategy => 
      strategy.id === id ? { ...strategy, ...updates } : strategy
    )
  })),
  
  deleteStrategy: (id) => set((state) => ({
    strategies: state.strategies.filter(strategy => strategy.id !== id)
  })),
  
  setCurrentStrategy: (strategy) => set({ currentStrategy: strategy }),
  
  setStrategyTemplates: (templates) => set({ strategyTemplates: templates }),
  
  setLoading: (loading) => set({ isLoading: loading }),
  
  setError: (error) => set({ error })
}));