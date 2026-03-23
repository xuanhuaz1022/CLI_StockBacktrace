#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>
#include <iostream>
#include <queue>
#include <memory>

class ThreadPool
{
private:
  std::queue<std::shared_ptr<std::function<void()>>> tasks_;
  std::vector<std::thread> threads_;
  std::mutex mutex_;
  std::condition_variable condition_;
  bool stop_;

public:
  ThreadPool(size_t num_threads);
  ~ThreadPool();

  template <class F, class... Args>
  auto enqueue(F &&f, Args &&...args) -> std::future<typename std::invoke_result<F, Args...>::type>
  {
    using return_type = typename std::invoke_result<F, Args...>::type;

    // 创建一个打包好的任务，包含返回值占位符
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();

    {
      std::unique_lock<std::mutex> lock(mutex_);
      auto task_func = std::make_shared<std::function<void()>>(
          [task]()
          { (*task)(); });
      tasks_.push(task_func);
    }
    condition_.notify_one();
    return res;
  }

  void shutdown();
};

#endif
