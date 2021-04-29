
#ifndef _SPI_MESSAGE_QUEUE_
#define _SPI_MESSAGE_QUEUE_

#include "TextLogging.hxx"

#include <cstring>

#include "BinaryUtil.hxx"
#include "ConstantsCommon.hpp"
#include "RemoteMessageHeader.hpp"
#include "SpiPayload.hpp"

namespace AmpedUp
{
    template<uint32_t messageCapacity>
    class SpiMessageQueue
    {
    public:

        SpiMessageQueue() = default;

        ~SpiMessageQueue() = default;

        void reset()
        {
            frontIndex_ = 0;
            usedSize_ = 0;
            messageCount_ = 0;
        }

        SpiPayload stageEnqueue(RemoteMessageSize_t size)
        {
            uint32_t sizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            *sizeTagPtr = size;

            uint32_t payloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), size);
            return SpiPayload(buffer_ + payloadIndex, size);
        }

        SpiPayload getStagedMessage()
        {
            // Find the size of the staged message
            uint32_t sizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            RemoteMessageSize_t size = *sizeTagPtr;

            // Find the payload for the staged message
            uint32_t payloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), size);
            BinaryUtil::byte_t* payloadPtr = buffer_ + payloadIndex;

            return SpiPayload(payloadPtr, size);
        }

        SpiPayload resizeStagedMessage(RemoteMessageSize_t newSize, RemoteMessageSize_t headUsed, RemoteMessageSize_t tailUsed)
        {
            // Find the size of the existing staged message
            uint32_t sizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            RemoteMessageSize_t oldSize = *sizeTagPtr;

            // Find the payload for the existing staged message
            uint32_t oldPayloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), oldSize);
            BinaryUtil::byte_t* oldPayloadPtr = buffer_ + oldPayloadIndex;

            // Update the size to the new size and find the new location for the message
            *sizeTagPtr = newSize;
            uint32_t newPayloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), newSize);
            BinaryUtil::byte_t* newPayloadPtr = buffer_ + newPayloadIndex;

            // Copy the head data if necessary
            if (headUsed > 0 && newPayloadIndex != oldPayloadIndex)
            {
                memcpy(newPayloadPtr, oldPayloadPtr, headUsed);
            }

            // Copy the tail data if necessary
            if (tailUsed > 0)
            {
                BinaryUtil::byte_t* oldPayloadTailPtr = oldPayloadPtr + oldSize - tailUsed;
                BinaryUtil::byte_t* newPayloadTailPtr = newPayloadPtr + newSize - tailUsed;
                memmove(newPayloadTailPtr, oldPayloadTailPtr, tailUsed);
            }
            
            return SpiPayload(newPayloadPtr, newSize);  
        }

        SpiPayload trimStagedMessage(const BinaryUtil::byte_t* dataWithinStagedPayload, RemoteMessageSize_t size)
        {
            // Find the size tag for the existing staged message
            uint32_t sizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);

            // Update the size tag to reflect the new size
            *sizeTagPtr = size;

            // Find the new location for the staged message
            uint32_t newPayloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), size);
            BinaryUtil::byte_t* newPayloadPtr = buffer_ + newPayloadIndex;
            memmove(newPayloadPtr, dataWithinStagedPayload, size);

            return SpiPayload(newPayloadPtr, size);
        }

        void commitStagedMessage()
        {
            uint32_t sizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            usedSize_ += sizeof(RemoteMessageSize_t) + *sizeTagPtr;
            messageCount_++;
        }

        SpiPayload peekFront()
        {
            uint32_t sizeTagIndex = getBlockIndexFollowing(frontIndex_, sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            RemoteMessageSize_t size = *sizeTagPtr;

            uint32_t payloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), size);

            return SpiPayload(buffer_ + payloadIndex, size);
        }

        void dequeue()
        {
            uint32_t sizeTagIndex = getBlockIndexFollowing(frontIndex_, sizeof(RemoteMessageSize_t));
            RemoteMessageSize_t* sizeTagPtr = reinterpret_cast<RemoteMessageSize_t*>(buffer_ + sizeTagIndex);
            RemoteMessageSize_t size = *sizeTagPtr;

            uint32_t payloadIndex = getBlockIndexFollowing(sizeTagIndex + sizeof(RemoteMessageSize_t), size);

            usedSize_ -= sizeof(RemoteMessageSize_t) + size;
            if (usedSize_ > 0)
            {
                frontIndex_ = (payloadIndex + size) % CAPACITY_BYTES;
            }
            else
            {
                frontIndex_ = 0;
            }
            messageCount_--;
        }

        bool empty()
        {
            return messageCount_ == 0;
        }

        RemoteMessageSize_t getMaximumEnqueueSize() const
        {
            uint32_t endSizeTagIndex = getBlockIndexFollowing(endIndex(), sizeof(RemoteMessageSize_t));
            uint32_t endPayloadStartIndex = BinaryUtil::bytesFillWords(endSizeTagIndex + sizeof(RemoteMessageSize_t));
            uint32_t endPayloadMaxSize = CAPACITY_BYTES - endPayloadStartIndex;

            uint32_t frontPayloadMaxSize = getBlockIndexFollowing(frontIndex_, sizeof(RemoteMessageSize_t));

            uint32_t maxPayloadSize = std::max(endPayloadMaxSize, frontPayloadMaxSize);
            RemoteMessageSize_t maxAllowableSize = std::min(maxPayloadSize, static_cast<uint32_t>(Constants::REMOTE_MESSAGE_MAX_SIZE));
            return maxAllowableSize;
        }

        bool canEnqueue(RemoteMessageSize_t size) const
        {
            return size <= getMaximumEnqueueSize();
        }

        uint32_t getMessageCount() const
        {
            return messageCount_;
        }

    private:
        static constexpr uint32_t CAPACITY_BYTES = BinaryUtil::bytesFillWords(messageCapacity * Constants::REMOTE_MESSAGE_MAX_SIZE);

        alignas(BinaryUtil::word_t) BinaryUtil::byte_t buffer_[CAPACITY_BYTES];
        volatile uint32_t frontIndex_{};
        volatile uint32_t usedSize_{};
        volatile uint32_t messageCount_{};

        uint32_t endIndex() const
        {
            return (frontIndex_ + usedSize_) % CAPACITY_BYTES;
        }

        uint32_t getBlockIndexFollowing(uint32_t index, uint32_t blockSize) const
        {
            // Everything must start on a word boundary
            index = BinaryUtil::bytesFillWords(index);
            return (index + blockSize <= CAPACITY_BYTES) ? index : 0;
        }

    };
};

#endif