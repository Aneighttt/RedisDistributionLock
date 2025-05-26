#include "RedisConnectionManager.h"
#include <stdexcept>

// 初始化静态成员
std::unique_ptr<RedisConnectionManager> RedisConnectionManager::instance = nullptr;
std::mutex RedisConnectionManager::mutex;

RedisConnectionManager& RedisConnectionManager::getInstance() {
    if (instance == nullptr) {
        std::lock_guard<std::mutex> lock(mutex);
        if (instance == nullptr) {
            instance = std::unique_ptr<RedisConnectionManager>(new RedisConnectionManager());
        }
    }
    return *instance;
}

void RedisConnectionManager::initialize(const std::string& host, int port, 
                                      const std::string& password, int db,
                                      int connect_timeout, int socket_timeout,
                                      int pool_size) {
    std::lock_guard<std::mutex> lock(mutex);
    
    // 配置连接选项
    conn_opts.host = host;
    conn_opts.port = port;
    conn_opts.password = password;
    conn_opts.db = db;
    
    if (connect_timeout > 0) {
        conn_opts.connect_timeout = std::chrono::milliseconds(connect_timeout);
    }
    
    if (socket_timeout > 0) {
        conn_opts.socket_timeout = std::chrono::milliseconds(socket_timeout);
    }
    
    // 配置连接池选项
    pool_opts.size = pool_size;
    pool_opts.wait_timeout = std::chrono::milliseconds(100);
    pool_opts.connection_lifetime = std::chrono::minutes(10);
    
    try {
        redis = std::make_unique<sw::redis::Redis>(conn_opts, pool_opts);
        initialized.store(true);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to initialize Redis connection: " + std::string(e.what()));
    }
}

sw::redis::Redis& RedisConnectionManager::getConnection() {
    if (!initialized.load()) {
        throw std::runtime_error("Redis connection not initialized");
    }
    return *redis;
}

bool RedisConnectionManager::isConnected() const {
    if (!initialized.load() || !redis) {
        return false;
    }
    
    try {
        // 尝试执行PING命令来检查连接
        redis->ping();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void RedisConnectionManager::reconnect() {
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (!initialized.load()) {
            throw std::runtime_error("Cannot reconnect: Redis connection not initialized");
        }
    }
    
    try {
        // 使用保存的连接参数重新初始化
        redis = std::make_unique<sw::redis::Redis>(conn_opts, pool_opts);
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to reconnect: " + std::string(e.what()));
    }
} 