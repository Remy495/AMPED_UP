
#ifndef _STATIC_QUEUE_
#define _STATIC_QUEUE_

#include <array>
#include <utility>

namespace AmpedUp
{

    template<typename T, uint32_t CAPACITY>
    class StaticQueue
    {
    public:
        StaticQueue() = default;

        ~StaticQueue() = default;

        uint32_t getSize() const
        {
            return size_;
        }

        uint32_t getCapacity() const
        {
            return CAPACITY;
        }

        uint32_t getRemainingSpace() const
        {
            return CAPACITY - size_;
        }

        bool isEmpty() const
        {
            return size_ == 0;
        }

        bool isFull() const
        {
            return size_ == CAPACITY
        }

        void enqueue(const T& element)
        {
            if (!isFull())
            {
                // Add the new element
                storage_[getEndPos()] = element;
                // Move back the end of the queue
                size_++;
            }
        }

        T dequeue()
        {
            T element;
            if (!isEmpty())
            {
                // Extract the front element
                element = std::move_if_noexcept(storage_[frontPos_]);
                // Move up the front of the queue
                frontPos_ = (frontPos_ + 1) % CAPACITY;
                size_--;
            }

            return element;
        }

        void clear()
        {
            frontPos_ = 0;
            size_ = 0;
        }

        T& peekFront()
        {
            return storage_[frontPos_];
        }

        const T& peekFont() const
        {
            return storage_[frontPos_];
        }

        T& peekBack()
        {
            return storage_[getBackPos()];
        }

        const T& peekBack() const
        {
            return storage_[getBackPos()];
        }

    private:
        std::array<T, CAPACITY> storage_{};

        uint32_t frontPos_{0};
        uint32_t size_{0};

        uint32_t getEndPos() const
        {
            return (frontPos_ + size_) % CAPACITY;
        }

        uint32_t getBackPos() const
        {
            // Last element of the queue is one before the "end" because of zero index
            // If the queue goes past the end of the internal array it wraps around
            return (frontPos_ + size_ - 1) % CAPACITY;
        }

    };

}

#endif