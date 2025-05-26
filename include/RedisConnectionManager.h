#pragma once

#include <sw/redis++/redis++.h>
#include <memory>
#include <string>
#include <mutex>
#include <atomic>

class RedisConnectionManager {
public:
    // 获取单例实例
    static RedisConnectionManager& getInstance();
    
    // 初始化Redis连接
    void initialize(const std::string& host = "127.0.0.1", 
                   int port = 6379,
                   const std::string& password = "",
                   int db = 0,
                   int connect_timeout = 0,
                   int socket_timeout = 0,
                   int pool_size = 20);
    
    // 获取Redis连接
    sw::redis::Redis& getConnection();
    
    // 检查连接是否有效
    bool isConnected() const;
    
    // 重新连接
    void reconnect();

    ~RedisConnectionManager() = default;
    
private:
    // 私有构造函数，防止外部创建实例
    RedisConnectionManager() = default;
    
    // 禁止拷贝和赋值
    RedisConnectionManager(const RedisConnectionManager&) = delete;
    RedisConnectionManager& operator=(const RedisConnectionManager&) = delete;
    
    // 单例实例
    static std::unique_ptr<RedisConnectionManager> instance;
    static std::mutex mutex;
    
    // Redis连接
    std::unique_ptr<sw::redis::Redis> redis;
    std::atomic<bool> initialized = false;
    
    // 连接参数
    sw::redis::ConnectionOptions conn_opts;
    sw::redis::ConnectionPoolOptions pool_opts;
};
 