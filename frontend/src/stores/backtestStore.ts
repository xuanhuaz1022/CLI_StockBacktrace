import { create } from 'zustand';
import type { BacktestTask, BacktestResult } from '../types';

interface BacktestStoreState {
  // 状态
  backtestTasks: BacktestTask[];
  runningTasks: BacktestTask[];
  currentResult: BacktestResult | null;
  isLoading: boolean;
  error: string | null;
  
  // 操作方法
  setBacktestTasks: (tasks: BacktestTask[]) => void;
  addBacktestTask: (task: BacktestTask) => void;
  updateBacktestTask: (id: string, updates: Partial<BacktestTask>) => void;
  deleteBacktestTask: (id: string) => void;
  setRunningTasks: (tasks: BacktestTask[]) => void;
  setCurrentResult: (result: BacktestResult | null) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
}

export const useBacktestStore = create<BacktestStoreState>((set) => ({
  // 初始状态
  backtestTasks: [],
  runningTasks: [],
  currentResult: null,
  isLoading: false,
  error: null,
  
  // 操作方法
  setBacktestTasks: (tasks) => set({ backtestTasks: tasks }),
  
  addBacktestTask: (task) => set((state) => ({
    backtestTasks: [...state.backtestTasks, task]
  })),
  
  updateBacktestTask: (id, updates) => set((state) => ({
    backtestTasks: state.backtestTasks.map(task => 
      task.id === id ? { ...task, ...updates } : task
    ),
    runningTasks: state.runningTasks.map(task => 
      task.id === id ? { ...task, ...updates } : task
    )
  })),
  
  deleteBacktestTask: (id) => set((state) => ({
    backtestTasks: state.backtestTasks.filter(task => task.id !== id),
    runningTasks: state.runningTasks.filter(task => task.id !== id)
  })),
  
  setRunningTasks: (tasks) => set({ runningTasks: tasks }),
  
  setCurrentResult: (result) => set({ currentResult: result }),
  
  setLoading: (loading) => set({ isLoading: loading }),
  
  setError: (error) => set({ error })
}));