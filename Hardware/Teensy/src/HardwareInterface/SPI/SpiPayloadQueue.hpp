

#ifndef _SPI_PAYLOAD_QUEUE_
#define _SPI_PAYLOAD_QUEUE_

#include "SpiPayload.hpp"
#include "Arduino.h"

namespace AmpedUp
{
    template<uint32_t capacity>
    class SpiPayloadQueue
    {
    public:

        SpiPayloadQueue() = default;

        ~SpiPayloadQueue() = default;

        void reset()
        {
            frontIndex_ = 0;
            usedSize_ = 0;
            for (uint32_t i = 0; i < capacity; i++)
            {
                payloads_[capacity].setUsedSize(0);
            }
        }

        SpiPayload& peekFront()
        {
            return payloads_[frontIndex_];
        }

        const SpiPayload& peekFront() const
        {
            return payloads_[frontIndex_];
        }

        void dequeue()
        {
            frontIndex_ = (frontIndex_ + 1) % capacity;
            usedSize_--;
        }

        SpiPayload& stageEnqueue()
        {
            uint32_t endIndex = (frontIndex_ + usedSize_) % capacity;
            return payloads_[endIndex];
        }

        void commitStagedEnqueue()
        {
            usedSize_++;
        }

        uint32_t getCapacity() const
        {
            return capacity;
        }

        uint32_t getSize() const
        {
            return usedSize_;
        }

        bool isEmpty() const
        {
            return usedSize_ == 0;
        }

        bool isFull() const
        {
            return usedSize_ >= capacity;
        }

private:

        SpiPayload payloads_[capacity]{};
        uint32_t frontIndex_{};
        uint32_t usedSize_{};
    };
}


#endif