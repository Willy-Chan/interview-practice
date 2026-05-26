
// there is a persistent consumer thread constantly polling the queue
//      spin waiting for some work on the queue (condition variable)
//      if there is something there, unlock and "consume" it whatever that happens to mean
//      check if that's the last item

// meanwhile, user can publically "push work" onto the queue
//      lock and push
//      notify condition variable (outside scope if same lock)

template <typename T>
struct DataWrapper
{
    T data{ };
    bool is_last_chunk{ false };
};

template <typename T, typename Callback>
class SPSC
{
public:
    SPSC(Callback callback) 
        : on_consume_(callback), // attach the custom callback to our internal data structure's on_consume_ function
          consumer_thread([this] { Consume(); }) // immediately launch our persistent consumer thread that waits for data
    {   
        
    }

    ~SPSC()
    {
        if (this->consumer_thread.joinable()) {
            this->consumer_thread.join();
        }
    }

    void PushWork(const DataWrapper<T>& wrapper)
    {
        {
            std::unique_lock<std::mutex>lock(this->queue_mutex_);
            // lock and push the item on the queue
            this->queue_.push(wrapper);
        }
        
        queue_has_items_.notify_one();
    }

private:
    Callback on_consume_;
    std::queue<DataWrapper<T>> queue_;
    std::thread consumer_thread{ };
    std::mutex queue_mutex_{ };
    std::condition_variable queue_has_items_{ };

    void Consume()
    {  
        while (true) {

            // wait until there is data
            std::unique_lock<std::mutex> lock(this->queue_mutex_);
            queue_has_items_.wait(lock, [this]{ return !this->queue_.empty(); });
            auto item = this->queue_.front();
            this->queue_.pop();

            lock.unlock();   // unlock so producer can keep going now

            on_consume_(item.data);


            if (item.is_last_chunk) {
                break;
            }
        }
    }
};