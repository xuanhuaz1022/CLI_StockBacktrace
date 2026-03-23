#include "thread_pool.h"

ThreadPool::ThreadPool(size_t num_threads)
{
  if (num_threads == 0)
  {
    throw std::invalid_argument("num_threads must be greater than 0");
  }
  threads_.reserve(num_threads);
  stop_ = false;

  for (size_t i = 0; i < num_threads; ++i)
  {
    threads_.emplace_back([this]()
                          {
      while (true)
      {
        std::shared_ptr<std::function<void()>> task_ptr;
        {
          // 1. 获取锁
          std::unique_lock<std::mutex> lock(mutex_);
          this->condition_.wait(lock, [this]
                                { return this->stop_ || !tasks_.empty(); });
          if (this->stop_)
          {
            return;
          }
          task_ptr = std::move(this->tasks_.front());
          this->tasks_.pop();
        }
        (*task_ptr)();
      } });
  }
}

ThreadPool::~ThreadPool()
{
  stop_ = true;
  condition_.notify_all();
  for (auto &thread : threads_)
  {
    if (thread.joinable())
    {
      thread.join();
    }
  }
}

void ThreadPool::shutdown()
{
  stop_ = true;
  condition_.notify_all();
}
