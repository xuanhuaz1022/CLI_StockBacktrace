import { create } from 'zustand';
import type { DataFile, DataInfo } from '../types';

interface DataStoreState {
  // 状态
  dataFiles: DataFile[];
  currentData: DataInfo | null;
  isLoading: boolean;
  error: string | null;

  // 操作方法
  setDataFiles: (files: DataFile[]) => void;
  addDataFile: (file: DataFile) => void;
  updateDataFile: (id: string, updates: Partial<DataFile>) => void;
  deleteDataFile: (id: string) => void;
  setCurrentData: (data: DataInfo | null) => void;
  setLoading: (loading: boolean) => void;
  setError: (error: string | null) => void;
}

export const useDataStore = create<DataStoreState>((set) => ({
  // 初始状态
  dataFiles: [],
  currentData: null,
  isLoading: false,
  error: null,

  // 操作方法
  setDataFiles: (files) => set({ dataFiles: files }),

  addDataFile: (file) => set((state) => ({
    dataFiles: [...state.dataFiles, file]
  })),

  updateDataFile: (id, updates) => set((state) => ({
    dataFiles: state.dataFiles.map(file =>
      file.id === id ? { ...file, ...updates } : file
    )
  })),

  deleteDataFile: (id) => set((state) => ({
    dataFiles: state.dataFiles.filter(file => file.id !== id)
  })),

  setCurrentData: (data) => set({ currentData: data }),

  setLoading: (loading) => set({ isLoading: loading }),

  setError: (error) => set({ error })
}));