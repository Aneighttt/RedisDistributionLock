#include <queue>
#include <mutex>
#include <condition_variable>
template<typename T>
class BlockingQueue{
public:
    BlockingQueue(bool nonBlock = false): nonBlock(nonBlock){}

    void Push(const T& val){
        std::unique_lock<std::mutex> lock(producerMtx);
        producerQueue.push(val);
        notEmpty.notify_one();
    }
    bool Pop(T& val){
        std::unique_lock<std::mutex> lock(consumerMtx);
        if(consumerQueue.empty() && SwapQueue() == 0){
            return false;
        }
        val = consumerQueue.front();
        consumerQueue.pop();
        return true;
    }
    void Cancel(){
        {
            std::lock_guard<std::mutex> lock(producerMtx);
            nonBlock = true;
        }
        notEmpty.notify_all();
    }
private:
    int SwapQueue(){
        std::unique_lock<std::mutex> lock(producerMtx);
        notEmpty.wait(lock,[this](){return nonBlock || !producerQueue.empty();});
        swap(consumerQueue,producerQueue);
        return consumerQueue.size();
    }
    std::queue<T> consumerQueue;
    std::queue<T> producerQueue;
    bool nonBlock;
    std::mutex consumerMtx;
    std::mutex producerMtx;
    std::condition_variable notEmpty;
};