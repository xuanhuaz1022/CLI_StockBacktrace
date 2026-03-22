
#include "thread_pool.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include <thread>

int main() {
    // 创建一个包含4个线程的线程池
    ThreadPool pool(4);
    
    // 用于存储任务的future
    std::vector<std::future<int>> results;
    
    // 提交10个任务到线程池
    for (int i = 0; i < 10; ++i) {
        results.emplace_back(pool.enqueue([i] {
            std::cout << "Task " << i << " starting\n";
            std::this_thread::sleep_for(std::chrono::seconds(1));
            std::cout << "Task " << i << " completed\n";
            return i * i;
        }));
    }
    
    // 收集所有任务的结果
    int sum = 0;
    for (auto &result : results) {
        sum += result.get();
    }
    
    std::cout << "Sum of results: " << sum << std::endl;
    
    return 0;
}
