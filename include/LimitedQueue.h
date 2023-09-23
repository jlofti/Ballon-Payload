#include <memory>
#include <queue>
template <typename T>
class LimitedQueue_t
{
private:
    int count = 0;
    int max;
    std::shared_ptr<std::queue<T>> queue;

public:
    LimitedQueue_t<T>()
    {
        LimitedQueue_t<T>(100);
    };
    LimitedQueue_t<T>(int max_)
    {
        this->max = max_;
        this->queue = std::shared_ptr<std::queue<T>>(new std::queue<T>);
    };

    /*
     * Pushs object to queue and increases count
     *
     */
    void push(T item_)
    {
        if (this->count < this->max)
        {
            this->queue->push(item_);
            this->count++;
        }

        else
        {
            this->flush();
        }
    };

    /*
     * Pops off of queue and decrements counter
     *
     * @return object at front of queue
     */
    T pop()
    {
        T item_ = this->queue->front();
        this->queue->pop();
        count--;
        return item_;
    };

    /*
     * Checks if queue is empty
     *
     * Return true or false if queue is empty
     */
    bool isEmpty()
    {
        return queue->empty();
    };

    /*
     * Erases entire queue is max is hit
     *
     */
    void flush()
    {
        while (!this->isEmpty())
        {
            this->queue->front();
            this->queue->pop();
            this->count--;
        }

        this->count == 0;
    };
};