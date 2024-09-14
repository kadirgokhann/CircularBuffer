template<typename T>
CircularBuffer<T>::CircularBuffer(size_t size) :
    head_(0),
    tail_(0),
    capacity_(size * sizeof(T)),
    element_size_(sizeof(T)),
    full_(false)
{
    buffer_ = std::make_unique<char[]>(capacity_); // Allocate raw memory as bytes
    PrintState("Initial");
}
// (Producer)
template<typename T>
void CircularBuffer<T>::Put(T& item) 
{
    //Producer acquires mutex (first lock).
    std::unique_lock<std::mutex> lock(mutex_);

    // Checks if the buffer is full:
    // If full, waits on condition variable (mutex is temporarily released).
    // When notified, the producer is woken up and re-acquires the mutex.
    not_full_.wait(lock, [this]() { return !full_; });
    //Writes to the buffer and updates the state.
    std::memcpy(&buffer_[head_], &item, element_size_);
    head_ = (head_ + element_size_) % capacity_;

    if (head_ == tail_) 
        full_ = true;
    PrintState("Producer");

    // Notifies the consumer that new data is available.
    // No need to unlock manually since the destructor of std::unique_lock will handle it.
    not_empty_.notify_one(); 
}

// (Consumer)
template<typename T>
T CircularBuffer<T>::Get() 
{
    // Consumer acquires the mutex lock to ensure exclusive access
    std::unique_lock<std::mutex> lock(mutex_);
    // Checks if the buffer is empty:
    // If the buffer is empty, wait on condition variable (mutex is temporarily released) for the producer to add an item
    // When notified, the consumer is woken up and re-acquires the mutex.
    not_empty_.wait(lock, [this]() { return full_ || head_ != tail_; });
    
    // Retrieve the item from the buffer at the current tail position
    T item;
    std::memcpy(&item, &buffer_[tail_], element_size_);

    // Advance the tail index (wrap around if necessary)
    tail_ = (tail_ + element_size_) % capacity_;

    // Mark the buffer as not full since we just removed an item
    full_ = false;

    PrintState("Consumer");

    // Notify the producer that space is available
    not_full_.notify_one();
    // No need to unlock manually since the destructor of std::unique_lock will handle it.

    return item;
}

template<typename T>
void CircularBuffer<T>::PrintState(const std::string& role) 
{
    std::cout << "| " << std::setw(10) << role << " | Buffer State: ";
    for (size_t i = 0; i < capacity_; i += element_size_) 
    {
        if (i == head_ && i == tail_)
            std::cout << "(H&T)_ "; // Head and tail at the same position
        else if (i == head_)
            std::cout << "(H)_ ";  // Head position
        else if (i == tail_)
            std::cout << "(T)_ ";  // Tail position
        else
            std::cout << "_ ";     // Empty space
    }
    std::cout << "\n";
}

template<typename T>
CircularBuffer<T>::~CircularBuffer() 
{
    // unique_ptr automatically cleans up the allocated memory
}
