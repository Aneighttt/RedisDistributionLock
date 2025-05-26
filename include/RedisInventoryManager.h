#pragma once

#include "RedisConnectionManager.h"
#include <string>
#include <mutex>
#include <memory>

class RedisInventoryManager {
public:
    // 构造函数
    RedisInventoryManager();
    
    // 获取库存
    int getStock(const std::string& productId);
    
    // 增加库存
    bool increaseStock(const std::string& productId, int quantity);
    
    // 减少库存
    bool decreaseStock(const std::string& productId, int quantity);
    
    // 设置库存
    bool setStock(const std::string& productId, int quantity);
    
    // 检查库存是否足够
    bool checkStock(const std::string& productId, int quantity);

private:
    // 生成Redis键
    std::string getKey(const std::string& productId) const;
    
    // Redis连接管理器
    RedisConnectionManager& redisManager;
    
    // 互斥锁，用于保护库存操作
    std::mutex mutex;
}; 