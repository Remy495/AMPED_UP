

#ifndef _CIRCULAR_QUEUE_
#define _CIRCULAR_QUEUE_

#include <ostream>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "RemoteMessageHeader.hpp"
#include "ConstantsLocal.hpp"
#include "ConstantsCommon.hpp"

namespace AmpedUp
{

class CircularQueue
{
public:

    bool initialize();

    void readRaw(uint8_t* destination, uint32_t size);

    void writeRaw(const uint8_t* source, uint32_t size);

    void readMessageData(uint8_t* destination, RemoteMessageSize_t size);

    void writeNewMessageData(const uint8_t* source, RemoteMessageSize_t totalMessageSize, RemoteMessageSize_t fragmentMessageSize);

    void writeContinuingMessageData(const uint8_t* source, RemoteMessageSize_t fragmentMessageSize);

    RemoteMessageSize_t getFrontMessageSize();

    RemoteMessageSize_t getFrontMessageAvailableSize();

    uint32_t getUsedSize();

    uint32_t getFreeSize();

    bool hasData();

    bool hasMessage();

    bool canWriteRaw(uint32_t size);

    bool canWriteNewMessageData(RemoteMessageSize_t fragmentMessageSize);

    bool canWriteContinuingMessageData(RemoteMessageSize_t fragmentMessageSize);

    void clear();

    void print(std::ostream& os);

private:

    static constexpr uint32_t BUFFER_SIZE = Constants::REMOTE_MESSAGE_MTU * Constants::MINIMUM_QUEUEABLE_MESSAGES;

    uint8_t buffer_[BUFFER_SIZE]{0};
    uint32_t frontIndex_{};
    uint32_t usedSize_{};

    SemaphoreHandle_t generalLock_{};

    uint32_t getEndIndex();

    void incrementFront(uint32_t amount);

    void peekBuffer(uint8_t* destination, uint32_t size, uint32_t offset = 0) const;

    void readBuffer(uint8_t* destination, uint32_t size, uint32_t offset = 0);

    void writeBuffer(const uint8_t* source, uint32_t size);

    void overwriteBufferFront(const uint8_t* source, uint32_t size);

    void popBuffer(uint32_t size);


};


}

std::ostream& operator<<(std::ostream& os, AmpedUp::CircularQueue& circularQueue);


#endif