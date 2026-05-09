#pragma once
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool {
public:
  explicit ThreadPool(int num_threads);
  ~ThreadPool();

  // 提交任务，callable 对象（lambda / 函数指针都行）
  void submit(std::function<void()> task);

private:
  void worker(); // 每个工作线程执行的函数

  std::vector<std::thread> threads_;        // 工作线程
  std::queue<std::function<void()>> tasks_; // 任务队列
  std::mutex mutex_;                        // 保护 tasks_
  std::condition_variable cv_;              // 通知有新任务
  bool stop_;                               // 停止标志
};