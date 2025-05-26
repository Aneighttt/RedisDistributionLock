#include "UserService.hpp"
#include <string>
#include <memory>
#include <iostream>

void UserService::TestReEntry(){
    auto& redis = m_redisManager.getConnection();
    {
        auto redisLock = m_factory.getDistributionLock(redis, "REDIS");
        redisLock->Lock();
    }
}

oatpp::Object<StatusDto> UserService::decreaseStock(const oatpp::String& stockId){
    auto& redis = m_redisManager.getConnection();
    // 创建锁对象，自动获取锁
    auto redisLock = m_factory.getDistributionLock(redis, "REDIS");
    redisLock->Lock();
    auto status = StatusDto::createShared();
    try {
        //TestReEntry();
        auto stockOpt = redis.get(static_cast<std::string>(stockId));
        if (!stockOpt) {
            throw std::runtime_error("Stock not found");
        }
        int stock = std::stoi(*stockOpt);
        if(stock > 0){
            redis.decrby(static_cast<std::string>(stockId), 1);
            status->status = "ok";
            status->code = 200;
            status->message = "Decrease! remains: " + std::to_string(stock - 1) + " serverPort: 8000!";
        } else {
            status->status = "fail";
            status->code = 400;
            status->message = "卖完了!";
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("Failed to decrease stock: " + std::string(e.what()));
    }
    return status;
}