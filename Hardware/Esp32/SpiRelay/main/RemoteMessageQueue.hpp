

#ifndef _REMOTE_MESSAGE_QUEUE_
#define _REMOTE_MESSAGE_QUEUE_

#include <iostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "RemoteMessage.hpp"

namespace AmpedUp
{

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Stores a queue of messages sent or received over SPI. Provides basic thread safety with some caviots, see
///        individual method descriptions for details
/// @details This class is designed to be used with minimal copying. When enqueuing and message, rather than building
///          the message in a local variable and then copying it into the queue, you can call stageEnqueue() to get a
///          reference to a message in a staging area, ready to be enqueued. This can be modified appropriately and then
///          committed to the queue with commitStagedEnqueue()
///
template<uint32_t capacity>
class RemoteMessageQueue
{
public:

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Create an SPI message queue
    ///
    RemoteMessageQueue() = default;

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Initialize this SPI message queue. This must be successfully called before doing any operations with the
    ///        queue
    ///
    /// @return True if the queue was successfully initialized, otherwise false
    ///
    bool initialize()
    {
        bool status = true;

        // Create the enqueue lock. This semaphore is used to ensure that messages are not enqueued while there is a 
        // staged messages already being enqueued. Note that dequeues operations are not affected when this lock is held
        enqueueLock_ = xSemaphoreCreateMutex();
        if (enqueueLock_ == 0)
        {
            status = false;
        }

        // Create the total lock. This semaphore is used to prevent race conditions on size_ and frontPos_ 
        if (status)
        {
            generalLock_ = xSemaphoreCreateMutex();
            if (generalLock_ == 0)
            {
                status = false;
            }
        }

        // Create the free space count semaphore. Keeps track of the number of free spaces in the queue. This prevents messages from being enqueued when the queue is full
        if (status)
        {
            freeSpaceCount_ = xSemaphoreCreateCounting(capacity, capacity);
            if (freeSpaceCount_ == 0)
            {
                status = false;
            }
        }

        // Create the used space count semaphore. Keeps track of the number of taken spaces in the queue. This prevents messages from being dequeued when the queue is empty
        if (status)
        {
            usedSpaceCount_ = xSemaphoreCreateCounting(capacity, 0);
            if (usedSpaceCount_ == 0)
            {
                status = false;
            }
        }

        isInitialized_ = status;
        return status;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Stage a new message to be added to the queue.
    ///
    /// @return A reference to the message staged to be enqueued. 
    ///
    /// @note Only one message can be staged for enqueuing at a time. Any call to stageEnqueue() while there is already
    ///       a message staged will block until the existing staged message is committed or canceled.
    ///
    /// @note Don't make any assumptions about the state of the RemoteMessage returned by this method. It is not
    ///       initialized to any particular value.
    ///
    RemoteMessage& stageEnqueue()
    {
        // Ensure there is enough room to enqueue something (and reserve a space for the staged item)
        xSemaphoreTake(freeSpaceCount_, portMAX_DELAY);

        // Ensure that no one else can enqueue while there is a staged enqueue
        xSemaphoreTake(enqueueLock_, portMAX_DELAY);

        // Hold general lock while actually getting the reserved space
        xSemaphoreTake(generalLock_, portMAX_DELAY);
        RemoteMessage& stagedMessage = messages[getEndPos()];
        xSemaphoreGive(generalLock_);

        return stagedMessage;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Get the currently staged message
    ///
    /// @return A reference to the message staged to be enqueued. 
    ///
    /// @note Calling this method while there is no staged message is undefined behavior.
    ///
    RemoteMessage& getStagedMessage()
    {
        // Hold general lock while retrieving the reserved space
        xSemaphoreTake(generalLock_, portMAX_DELAY);
        RemoteMessage& stagedMessage = messages[getEndPos()];
        xSemaphoreGive(generalLock_);

        return stagedMessage;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Enqueue the current staged message at the end of the queue.
    ///
    void commitStagedEnqueue()
    {
        // Hold the general lock while actually committing the staged enqueue
        xSemaphoreTake(generalLock_, portMAX_DELAY);
        size_++;
        xSemaphoreGive(generalLock_);

        // Give up the enqueue lock
        xSemaphoreGive(enqueueLock_);
        // Mark the reserved space as taken (i.e. a space which can be dequeued)
        xSemaphoreGive(usedSpaceCount_);
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Discard the current staged message
    ///
    void cancelStagedEnqueue()
    {
        // Give up enqueue lock
        xSemaphoreGive(enqueueLock_);
        // Give back free space that was reserved
        xSemaphoreGive(freeSpaceCount_);
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Get a reference to the message currently at the front of the queue. Blocks if the queue is currently
    ///        empty.
    ///
    /// @return A reference to the message at the front of the queue
    ///
    /// @note IMPORTANT: The thread synchronization for this class is designed to support multiple enqueuers but not
    ///                  necessarily multiple dequeuers. If you call peekFront() and then another thread calls dequeue()
    ///                  the when you are still using the reference you got there will be problems.
    ///
    RemoteMessage& peekFront()
    {
        // Make sure there is at least one item in the queue
        xSemaphoreTake(usedSpaceCount_, portMAX_DELAY);

        // Hold the general lock while getting the front item
        xSemaphoreTake(generalLock_, portMAX_DELAY);
        RemoteMessage& front = messages[frontPos_];
        xSemaphoreGive(generalLock_);

        // Give back the used space, as we aren't actually dequeuing an item yet
        xSemaphoreGive(usedSpaceCount_);

        return front;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Remove the element at the front of the queue. Blocks if the queue is currently empty.
    ///
    void dequeue()
    {
        // Reserve an item to take from the queue
        xSemaphoreTake(usedSpaceCount_, portMAX_DELAY);

        // Hold the general lock while actually performing the dequeue
        xSemaphoreTake(generalLock_, portMAX_DELAY);
        frontPos_ = (frontPos_ + 1) % capacity;
        size_--;
        xSemaphoreGive(generalLock_);

        // Mark the dequeued space as free (i.e. a space where an enqueue can take place)
        xSemaphoreGive(freeSpaceCount_);
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Check whether the queue is currently empty
    ///
    bool isEmpty() const
    {
        return size_ == 0;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Check whether the queue is currently full.
    ///
    bool isFull() const
    {
        return size_ == capacity;
    }

    //////////////////////////////////////////////////////////////////////////////////
    ///
    /// @brief Get the number of messages currently in the queue
    ///
    uint32_t size() const
    {
        return size_;
    }

private:
    RemoteMessage messages[capacity]{};
    uint32_t frontPos_{};
    uint32_t size_{};
    bool isInitialized_{};

    SemaphoreHandle_t generalLock_{};
    SemaphoreHandle_t enqueueLock_{};
    SemaphoreHandle_t freeSpaceCount_{};
    SemaphoreHandle_t usedSpaceCount_{};

    // TODO: Initialize count semaphores, add to enqueue and dequeue calls

    uint32_t getEndPos() const
    {
        return (frontPos_ + size_) % capacity;
    }

    uint32_t getBackPos() const
    {
        // Last element of the queue is one before the "end" because of zero index
        // If the queue goes past the end of the internal array it wraps around
        return (frontPos_ + size_ - 1) % capacity;
    }
};

}


#endif