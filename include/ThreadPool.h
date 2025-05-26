#include "BlockingQueue.h"
#include <vector>
#include <thread>
#include <functional>

class ThreadPool{
public:
    ThreadPool(size_t threadNum){
        for(size_t i = 0; i<threadNum;i++){
            m_workers.emplace_back([this]()->void{
                Worker();
            });
        }
    }
    ~ThreadPool(){
        m_tasks.Cancel();
        for(auto& worker:m_workers){
            if(worker.joinable()){
                worker.join();
            }
        }
    }
    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args){
        std::function<void()> task = std::bind(std::forward<F>(f),std::forward<Args>(args)...);
        m_tasks.Push(task);
    }
private:
    void Worker(){
        while(true){
            std::function<void()> task;
            if(!m_tasks.Pop(task)){
                return;
            }
            task();
        }
    }
    BlockingQueue<std::function<void()>> m_tasks;
    std::vector<std::thread> m_workers;
};