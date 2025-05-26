#pragma once

#include "dto/StatusDto.hpp"
#include "dto/InventoryDto.hpp"
#include "RedisInventoryManager.h"
#include "oatpp/web/protocol/http/Http.hpp"
#include "oatpp/macro/component.hpp"
#include "lock/DistributionLockFactory.hpp"
#include "lock/RedisLock.hpp"


class UserService{
public:
    typedef oatpp::web::protocol::http::Status Status;
private:
    OATPP_COMPONENT(std::shared_ptr<RedisInventoryManager>, m_manager);
    RedisConnectionManager& m_redisManager;
    DistributionLockFactory m_factory;
public:
    UserService():m_redisManager(RedisConnectionManager::getInstance()), m_factory(){}
    oatpp::Object<StatusDto> decreaseStock(const oatpp::String& stockId);
    void TestReEntry();
};