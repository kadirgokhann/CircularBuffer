template<typename T>
CircularBuffer<T>::CircularBuffer(size_t size) :
                              buffer_(size),
                              head_(0),
                              tail_(0),
                              full_(false)
{
    PrintState("Initial");
}

// (Producer)
template<typename T>
void CircularBuffer<T>::Put(T item) 
{
    //Producer acquires mutex (first lock).
    std::unique_lock<std::mutex> lock(mutex_);

    // Checks if the buffer is full:
    // If full, waits on condition variable (mutex is temporarily released).
    // When notified, the producer is woken up and re-acquires the mutex.
    not_full_.wait(lock, [this]() { return !full_; });
    std::cout << "| Producer | Acquires | -- | Buffer Locked | LOCKED      |\n";

    //Writes to the buffer and updates the state.
    buffer_[head_] = item;
    head_          = (head_ + 1) % buffer_.size();

    if (head_ == tail_) 
        full_ = true;
    PrintState("Producer");

    // Notifies the consumer that new data is available.
    // No need to unlock manually since the destructor of std::unique_lock will handle it.
    not_empty_.notify_one(); 
    std::cout << "| Producer | Releases | -- | Buffer Unlocked | UNLOCKED    |\n";
}

// (Consumer)
template<typename T>
T CircularBuffer<T>::Get() 
{
    // Consumer acquires the mutex lock to ensure exclusive access
    std::unique_lock<std::mutex> lock(mutex_);
    std::cout << "| --       | Waiting  | Consumer Acquires | Buffer Locked | LOCKED      |\n";

    // Checks if the buffer is empty:
    // If the buffer is empty, wait on condition variable (mutex is temporarily released) for the producer to add an item
    // When notified, the consumer is woken up and re-acquires the mutex.
    not_empty_.wait(lock, [this]() { return full_ || head_ != tail_; });
    
    // Retrieve the item from the buffer at the current tail position
    T item = buffer_[tail_];

    // Advance the tail index (wrap around if necessary)
    tail_ = (tail_ + 1) % buffer_.size();

    // Mark the buffer as not full since we just removed an item
    full_ = false;

    PrintState("Consumer");

    // Notify the producer that space is available
    not_full_.notify_one();
    // No need to unlock manually since the destructor of std::unique_lock will handle it.
    std::cout << "| --       | --       | Consumer Releases | Buffer Unlocked | UNLOCKED |\n";

    return item;
}

template<typename T>
void CircularBuffer<T>::PrintState(const std::string& role) 
{
    std::cout << "| " << std::setw(10) << role << " | Buffer State: ";
    for (size_t i = 0; i < buffer_.size(); ++i) 
    {
        if (i == head_ && i == tail_)
            std::cout << "(H&T)" << buffer_[i] << " "; // Head and tail at the same position
        else if (i == head_)
            std::cout << "(H)" << buffer_[i] << " ";  // Head position
        else if (i == tail_)
            std::cout << "(T)" << buffer_[i] << " ";  // Tail position
        else
            std::cout << buffer_[i] << " ";           // Buffer content
    }
    std::cout << "\n";
}
