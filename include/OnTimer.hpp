#pragma once
#include <map>
#include <functional>
#include <sys/epoll.h>
#include <string>
#include <memory>
#include <iostream>
#include "RedisConnectionManager.h"
#include <chrono>
#include <mutex>
#include <thread>
#include <atomic>
#include <unistd.h>
//通过完美转发通过AddTime接口将函数传入，封装成TimerTask最后执行
class Timer;
class TimerTask;

using TIMERTASK = std::shared_ptr<TimerTask>;
using CallBack = std::function<void(TIMERTASK)>;

class TimerTask : public std::enable_shared_from_this<TimerTask> {
    friend class Timer;
private:
    uint64_t m_addTime;
    uint64_t m_execTime;
    CallBack m_func;
public:
    TimerTask(uint64_t addTime, uint64_t execTime, CallBack func)
        :m_addTime(addTime),m_execTime(execTime),m_func(func){}
    uint64_t ExecTime() const{
        return m_execTime;
    }
    void Run(){
        m_func(shared_from_this());
        std::cout<<"定时任务执行"<< m_execTime<<std::endl;
    }
};

class OnTimer{
private:
    static std::unique_ptr<OnTimer> m_instance;
    static std::mutex m_mtx;
    std::thread m_worker;
    std::atomic<bool> m_running{false};
    std::mutex m_timeoutsMtx;  // 专门用于保护m_timeouts
    std::multimap<int,TIMERTASK> m_timeouts;
    int m_epfd{-1};  // epoll文件描述符

    OnTimer() {
        m_epfd = epoll_create(1);
        if (m_epfd < 0) {
            throw std::runtime_error("Failed to create epoll instance");
        }
        startWorker();
    }

    void startWorker() {
        m_running.store(true);
        m_worker = std::thread([this]() {
            epoll_event ev[64] = {};
            while(m_running) {
                int n = epoll_wait(m_epfd, ev, 64, WaitTime());
                if (n < 0) {
                    if (errno == EINTR) continue;
                    break;
                }
                Update(OnTimer::GetTick());
            }
        });
    }

public:
    static OnTimer* GetInstance() {
        if(m_instance == nullptr){
            std::lock_guard<std::mutex> lock(m_mtx);
            if(m_instance == nullptr){
                m_instance.reset(new OnTimer());
            }
        }
        return m_instance.get();
    }

    ~OnTimer() {
        m_running.store(false);
        if (m_worker.joinable()) {
            m_worker.join();
        }
        if (m_epfd >= 0) {
            close(m_epfd);
        }
        Clear();
    }

    void Clear() {
        std::lock_guard<std::mutex> lock(m_timeoutsMtx);
        m_timeouts.clear();
    }

    static uint64_t GetTick(){
        auto now = std::chrono::steady_clock::now();
        auto tmp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
        return tmp.count();
    }

    template<typename F, typename... ARGS>
    TIMERTASK AddTimeout(int expireTime, F&& f, ARGS&&... args) {
        auto func = std::bind(std::forward<F>(f), std::forward<ARGS>(args)...);
        auto now = GetTick();
        auto exec = now + expireTime;
        auto task = std::make_shared<TimerTask>(now, exec, func);
        try {
            std::lock_guard<std::mutex> lock(m_timeoutsMtx);
            m_timeouts.emplace(exec, task);
            std::cout<<"任务添加成功"<<exec<<std::endl;
            return task;
        } catch(const std::exception& e) {
            std::cerr << "任务添加失败: " << e.what() << std::endl;
            return nullptr;
        }
    }

    void DelTimeOut(TIMERTASK task) {
        std::lock_guard<std::mutex> lock(m_timeoutsMtx);
        auto range = m_timeouts.equal_range(task->ExecTime());
        for(auto itr = range.first; itr != range.second;) {
            if(itr->second == task) {
                std::cout<<"任务删除"<<task->ExecTime()<<std::endl;
                itr = m_timeouts.erase(itr);
                
            } else {
                ++itr;
            }
        }
    }

    int WaitTime() {
        std::lock_guard<std::mutex> lock(m_timeoutsMtx);
        auto now = GetTick();
        auto itr = m_timeouts.begin();
        if(itr == m_timeouts.end()) {
            return -1;
        }
        int diff = itr->first - now;
        return diff > 0 ? diff : 0;
    }

    void Update(uint64_t now) {
        std::lock_guard<std::mutex> lock(m_timeoutsMtx);
        for(auto itr = m_timeouts.begin(); itr != m_timeouts.end() && itr->first <= now;) {
            itr->second->Run();
            itr = m_timeouts.erase(itr);
        }
    }

    // 删除拷贝和移动操作
    OnTimer(const OnTimer&) = delete;
    OnTimer(OnTimer&&) = delete;
    OnTimer& operator=(const OnTimer&) = delete;
    OnTimer& operator=(OnTimer&&) = delete;
};

