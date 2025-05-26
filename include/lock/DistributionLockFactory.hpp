#pragma once
#include <string>
#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class AbstractLock{
public:
    AbstractLock() = default;
    virtual ~AbstractLock() = default;
    virtual void Lock() = 0;
    virtual bool TryLock() = 0;
    virtual bool TryLock(long time) = 0;
    virtual void Unlock() = 0; 
    AbstractLock(const AbstractLock&) = delete;
    AbstractLock& operator=(const AbstractLock&) = delete;
};
class RedisLock;

class DistributionLockFactory{
private:
    std::string lockName;
    std::string uuid;
public:
    DistributionLockFactory(){
        boost::uuids::random_generator gen;
        boost::uuids::uuid u = gen();
        uuid = boost::uuids::to_string(u);
    }
    template<typename T>
    std::unique_ptr<AbstractLock> getDistributionLock(T& instance, std::string lockType){
        if(lockType == "") return nullptr;
        if(lockType == "REDIS"){
            lockName = "RedisLock";
            return std::make_unique<RedisLock>(instance, lockName, uuid);
        }
        else if(lockType == "ZOOKEEPER"){
            lockName = "ZooKeeperLock";
            return nullptr;
        }
        return nullptr;
    }
};