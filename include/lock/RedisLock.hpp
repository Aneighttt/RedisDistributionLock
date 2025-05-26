#pragma once

#include "DistributionLockFactory.hpp"
#include <iostream>
#include <thread>
#include <atomic>
#include <chrono>
#include "OnTimer.hpp"
#include <sw/redis++/redis++.h>



class RedisLock : public AbstractLock {
private:
    std::string lockName;
    std::string uuidValue;
    long expireTime;
    sw::redis::Redis& m_redis;
    TIMERTASK m_renewTask;

    void renewExpire() {
        std::string script = "if redis.call('HEXISTS',KEYS[1],ARGV[1])==1 then \
                            return redis.call('expire',KEYS[1],ARGV[2]) \
                        else return 0 end";
        long long result = m_redis.eval<long long>(script, {lockName}, {uuidValue, std::to_string(expireTime)});
        if (result == 1) {
            // 续期成功，重新添加定时任务
            m_renewTask = OnTimer::GetInstance()->AddTimeout(expireTime * 1000 / 3, 
                [this]() { renewExpire(); });
        } else {
            std::cerr << "锁续期失败: " << lockName << std::endl;
        }
    }

public:
    RedisLock(sw::redis::Redis& redis, const std::string& key, const std::string& uuid) 
        : m_redis(redis), lockName(key) {
        uuidValue = uuid + ":" + std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
        expireTime = 50;
    }
    void Lock() override{
        TryLock();
    }
    bool TryLock() override{
        try{
            TryLock(-1);
        }catch(const std::exception& e){
            std::cerr << "TryLock failed: " << e.what() << std::endl;
            return false;
        }
        return true;
    }
    bool TryLock(long time) override{
        if(time == -1){
            std::string script = 
            "if redis.call('EXISTS',KEYS[1]) == 0 \
                or redis.call('HEXISTS',KEYS[1],ARGV[1]) == 1 then \
                    redis.call('HINCRBY',KEYS[1],ARGV[1],1) \
                    redis.call('EXPIRE',KEYS[1],ARGV[2]) \
                    return 1 \
            else return 0 end";
            std::cout<<"加锁:lockName:"<<lockName<<" uuidValue:"<<uuidValue<<std::endl;
            while(m_redis.eval<long long>(script, {lockName}, {uuidValue, std::to_string(expireTime)}) == 0){
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            // 首次添加续期任务
            m_renewTask = OnTimer::GetInstance()->AddTimeout(expireTime * 1000 / 3, 
                [this]() { renewExpire(); });
            return true;
        }
        return false;
    }
    void Unlock() override{
        if (m_renewTask) {
            OnTimer::GetInstance()->DelTimeOut(m_renewTask);
            m_renewTask.reset();
        }

        std::string script = 
                "if redis.call('HEXISTS',KEYS[1],ARGV[1]) == 0 then \
                    return nil \
                elseif redis.call('HINCRBY',KEYS[1],ARGV[1],-1) == 0 then \
                    return redis.call('DEL',KEYS[1]) \
                else return 0 end";
        std::cout<<"锁释放:lockName:"<<lockName<<" uuidValue:"<<uuidValue<<std::endl;
        long long flag = m_redis.eval<long long>(script, {lockName}, {uuidValue});
        if(flag == 0){
            std::cerr << "unlock failed"<< std::endl;
        }
    }
    ~RedisLock() {
        Unlock();
    }
};