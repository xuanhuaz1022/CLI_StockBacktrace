import { create } from 'zustand';
import type { LoadingState, Notification } from '../types';

interface UIStoreState {
  // 状态
  loading: LoadingState;
  notifications: Notification[];
  currentMenu: string;
  theme: 'light' | 'dark' | 'auto';
  sidebarCollapsed: boolean;
  
  // 操作方法
  setLoading: (loading: LoadingState) => void;
  showNotification: (notification: Omit<Notification, 'id'>) => void;
  removeNotification: (id: string) => void;
  setCurrentMenu: (menu: string) => void;
  setTheme: (theme: 'light' | 'dark' | 'auto') => void;
  toggleSidebar: () => void;
}

export const useUIStore = create<UIStoreState>((set) => ({
  // 初始状态
  loading: { isLoading: false },
  notifications: [],
  currentMenu: 'dashboard',
  theme: 'light',
  sidebarCollapsed: false,
  
  // 操作方法
  setLoading: (loading) => set({ loading }),
  
  showNotification: (notification) => set((state) => {
    const newNotification: Notification = {
      ...notification,
      id: Date.now().toString()
    };
    return {
      notifications: [...state.notifications, newNotification]
    };
  }),
  
  removeNotification: (id) => set((state) => ({
    notifications: state.notifications.filter(n => n.id !== id)
  })),
  
  setCurrentMenu: (menu) => set({ currentMenu: menu }),
  
  setTheme: (theme) => set({ theme }),
  
  toggleSidebar: () => set((state) => ({
    sidebarCollapsed: !state.sidebarCollapsed
  }))
}));