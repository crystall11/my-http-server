#include "thread_pool.h"

ThreadPool::ThreadPool(int num_threads):stop_(false){
    for(int i=0;i<num_threads;i++){
        threads_.emplace_back(&ThreadPool::worker,this);
    }
}
ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stop_=true;
    }
    cv_.notify_all();
    for(auto& t:threads_){
        t.join();
    }
 }

void ThreadPool::submit(std::function<void()> task){
    {
        std::unique_lock<std::mutex> lock(mutex_);
        tasks_.push(task);
    }
    cv_.notify_one();
}
void ThreadPool::worker(){
    while (true) {
        std::function<void()> task;
        {
            // 加锁，安全地检查队列
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] {
                return !tasks_.empty() || stop_;
            });
            if (stop_ && tasks_.empty()) return;  // 退出线程
            task = tasks_.front();
            tasks_.pop();
        }  // 出作用域，锁自动释放
        task();  // 锁外执行任务，不阻塞其他线程取任务
    }
}  