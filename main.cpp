#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <chrono>
#include <iomanip>

#include "CircBuffer.hpp"

void producer(CircularBuffer<int>& buffer, int items) 
{
    for (int i = 1; i < items; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        std::cout << "Producing item: " << i << std::endl;
        buffer.Put(i);
    }
}

void consumer(CircularBuffer<int>& buffer, int items) 
{
    for (int i = 1; i < items; ++i) 
    {
        int item = buffer.Get();
        std::cout << "Consuming item: " << item << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }
}

int main() 
{
    const size_t bufferSize = 10;
    const int itemsToProduce = 20;

    CircularBuffer<int> buffer(bufferSize);

    // Create producer and consumer threads
    std::thread producerThread(producer, std::ref(buffer), itemsToProduce);
    std::thread consumerThread(consumer, std::ref(buffer), itemsToProduce);

    // Wait for both threads to finish
    producerThread.join();
    consumerThread.join();

    return 0;
}
