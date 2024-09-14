#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>

template<typename T>
class CircularBuffer 
{
    std::unique_ptr<char[]> buffer_; 
    size_t         head_;
    size_t         tail_;
    size_t         capacity_;               // Total capacity of the buffer in bytes
    size_t         element_size_;           // Size of each element
    bool           full_;

    std::mutex mutex_;                  // Mutex to protect buffer access
    std::condition_variable not_full_;  // Condition variable for producer (buffer not full)
    std::condition_variable not_empty_; // Condition variable for consumer (buffer not empty)
public:
    CircularBuffer(size_t size);
    void Put(T& item);
    T    Get();
    void PrintState(const std::string &role);
    ~CircularBuffer();    
};

#include "CircBuffer.tpp"
