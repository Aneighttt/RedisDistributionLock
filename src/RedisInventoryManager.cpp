#include "RedisInventoryManager.h"
#include <stdexcept>

RedisInventoryManager::RedisInventoryManager():redisManager(RedisConnectionManager::getInstance()){}

std::string RedisInventoryManager::getKey(const std::string& productId) const {
    return productId;
}

int RedisInventoryManager::getStock(const std::string& productId) {
    try {
        auto& redis = redisManager.getConnection();
        auto stock = redis.get(getKey(productId));
        return stock ? std::stoi(*stock) : 0;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to get stock: " + std::string(e.what()));
    }
}

bool RedisInventoryManager::increaseStock(const std::string& productId, int quantity) {
    if (quantity <= 0) {
        return false;
    }
    
    try {
        std::lock_guard<std::mutex> lock(mutex);
        auto& redis = redisManager.getConnection();
        redis.incrby(getKey(productId), quantity);
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to increase stock: " + std::string(e.what()));
    }
}

bool RedisInventoryManager::decreaseStock(const std::string& productId, int quantity) {
    if (quantity <= 0) {
        return false;
    }
    
    try {
        std::lock_guard<std::mutex> lock(mutex);
        if (!checkStock(productId, quantity)) {
            return false;
        }
        
        auto& redis = redisManager.getConnection();
        redis.decrby(getKey(productId), quantity);
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to decrease stock: " + std::string(e.what()));
    }
}

bool RedisInventoryManager::setStock(const std::string& productId, int quantity) {
    if (quantity < 0) {
        return false;
    }
    
    try {
        auto& redis = redisManager.getConnection();
        redis.set(getKey(productId), std::to_string(quantity));
        return true;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to set stock: " + std::string(e.what()));
    }
}

bool RedisInventoryManager::checkStock(const std::string& productId, int quantity) {
    try {
        auto& redis = redisManager.getConnection();
        auto stock = redis.get(getKey(productId));
        return stock && std::stoi(*stock) >= quantity;
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to check stock: " + std::string(e.what()));
    }
} 