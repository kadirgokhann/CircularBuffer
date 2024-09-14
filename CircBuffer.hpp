#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

template<typename T>
class CircularBuffer 
{
    std::vector<T> buffer_;
    size_t         head_;
    size_t         tail_;
    bool           full_;

    std::mutex mutex_;                  // Mutex to protect buffer access
    std::condition_variable not_full_;  // Condition variable for producer (buffer not full)
    std::condition_variable not_empty_; // Condition variable for consumer (buffer not empty)
public:
    CircularBuffer(size_t size);
    void Put(T item);
    T    Get();
    void PrintState(const std::string &role);
};

#include "CircBuffer.tpp"