#ifndef DONUT_FRAME_BUFFER_H
#define DONUT_FRAME_BUFFER_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <stdexcept>

class DonutAVDataCacheException : public std::runtime_error {
public:
    DonutAVDataCacheException(const std::string& msg) : std::runtime_error(msg) {}
};

template <typename T>
class DonutAVDataCache {
public:

    DonutAVDataCache(int capacity) : capacity_(capacity), size_(0), shutdown_(false)
    {
        if (capacity_ <= 0)
        {
            throw DonutAVDataCacheException("Capacity must be greater than 0.");
        }
    }

    ~DonutAVDataCache()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        shutdown_ = true;
        not_empty_.notify_all();
        not_full_.notify_all();
    }

    void push(const T& item)
    {
        if (shutdown_)
        {
            throw DonutAVDataCacheException("Cannot push into a shutdown cache.");
        }
        std::unique_lock<std::mutex> lock(mutex_);
        while (size_ >= capacity_ && !shutdown_)
        {
            not_full_.wait(lock);
        }
        if (shutdown_)
        {
            return;
        }
        queue_.push(item);
        ++size_;
        not_empty_.notify_all();
    }


    T pop() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (queue_.empty() && !shutdown_)
        {
            not_empty_.wait(lock);
        }
        if (shutdown_)
        {
            return T();
        }
        T item = queue_.front();
        queue_.pop();
        --size_;
        not_full_.notify_all();
        return item;
    }


    void clearCache()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!queue_.empty())
        {
            queue_.pop();
            --size_;
        }
        not_full_.notify_all();
    }


    int getSize() const
    {
        return size_;
    }

    int getCapacity() const
    {
        return capacity_;
    }

    void setCapacity(int capacity)
    {
        if (capacity <= 0)
        {
            throw FrameCacheException("Capacity must be greater than 0.");
        }
        std::unique_lock<std::mutex> lock(mutex_);
        capacity_ = capacity;
        not_full_.notify_all();
    }

private:
    std::queue<T> queue_;
    std::mutex mutex_;
    std::condition_variable not_empty_;
    std::condition_variable not_full_;
    int capacity_;
    std::atomic<int> size_;
    bool shutdown_;
};

#endif