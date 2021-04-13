

#include <cstring>
#include <iostream>

#include "CircularQueue.hpp"
#include "BinaryUtil.hxx"


bool AmpedUp::CircularQueue::initialize()
{
    bool status = true;

    generalLock_ = xSemaphoreCreateMutex();
    if (generalLock_ == 0)
    {
        status = false;
    }

    return status;
}

void AmpedUp::CircularQueue::readRaw(uint8_t* destination, uint32_t size)
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    readBuffer(destination, size);

    xSemaphoreGive(generalLock_);
}

void AmpedUp::CircularQueue::writeRaw(const uint8_t* source, uint32_t size)
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    writeBuffer(source, size);

    xSemaphoreGive(generalLock_);
}

void AmpedUp::CircularQueue::readMessageData(uint8_t* destination, RemoteMessageSize_t size)
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    // Get the size of the front message in the queue
    RemoteMessageSize_t frontMessageSize;
    uint8_t* frontMessageSizeData = BinaryUtil::asRawData(frontMessageSize);
    peekBuffer(frontMessageSizeData, sizeof(RemoteMessageSize_t));

    if (size < frontMessageSize)
    {
        // If the amount of data to be read is less than the front message's size, we should essentially shorten the
        // message to what remains after the requested data is taken and move up the size tag.
        readBuffer(destination, size, sizeof(RemoteMessageSize_t));
        frontMessageSize -= size;
        overwriteBufferFront(frontMessageSizeData, sizeof(RemoteMessageSize_t));
    }
    else
    {
        // If the amount of data to be read is the full size of the front message, discard the front message's size tag.
        readBuffer(destination, size, sizeof(RemoteMessageSize_t));
        popBuffer(sizeof(RemoteMessageSize_t));
    }
    
    xSemaphoreGive(generalLock_);
}

void AmpedUp::CircularQueue::writeNewMessageData(const uint8_t* source, RemoteMessageSize_t totalMessageSize, RemoteMessageSize_t fragmentMessageSize)
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    // Write the size tag for the message
    writeBuffer(BinaryUtil::asRawData(totalMessageSize), sizeof(RemoteMessageSize_t));

    // Write the message data
    writeBuffer(source, fragmentMessageSize);

    xSemaphoreGive(generalLock_);
}

void AmpedUp::CircularQueue::writeContinuingMessageData(const uint8_t* source, RemoteMessageSize_t fragmentMessageSize)
{
    // Writing the continuation to an already started message is really the same as just writing raw data (since it
    // won't cross message boundaries anyway)
    writeRaw(source, fragmentMessageSize);
}

AmpedUp::RemoteMessageSize_t AmpedUp::CircularQueue::getFrontMessageSize()
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    RemoteMessageSize_t frontMessageSize = 0;
    if (usedSize_ >= sizeof(RemoteMessageSize_t))
    {
        peekBuffer(BinaryUtil::asRawData(frontMessageSize), sizeof(RemoteMessageSize_t));
    }

    xSemaphoreGive(generalLock_);

    return frontMessageSize;
}

AmpedUp::RemoteMessageSize_t AmpedUp::CircularQueue::getFrontMessageAvailableSize()
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    RemoteMessageSize_t frontMessageSize = 0;
    if (usedSize_ >= sizeof(RemoteMessageSize_t))
    {
        peekBuffer(BinaryUtil::asRawData(frontMessageSize), sizeof(RemoteMessageSize_t));

        if (frontMessageSize > usedSize_ - sizeof(RemoteMessageSize_t))
        {
            frontMessageSize = usedSize_ - sizeof(RemoteMessageSize_t);
        }
    }

    xSemaphoreGive(generalLock_);

    return frontMessageSize;
}

uint32_t AmpedUp::CircularQueue::getUsedSize()
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    uint32_t usedSizeTemp = usedSize_;

    xSemaphoreGive(generalLock_);

    return usedSizeTemp;
}

uint32_t AmpedUp::CircularQueue::getFreeSize()
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    uint32_t freeSizeTemp = BUFFER_SIZE - usedSize_;

    xSemaphoreGive(generalLock_);

    return freeSizeTemp;
}

bool AmpedUp::CircularQueue::hasData()
{
    return getUsedSize() > 0;
}

bool AmpedUp::CircularQueue::hasMessage()
{
    return getUsedSize() > sizeof(RemoteMessageSize_t);
}

bool AmpedUp::CircularQueue::canWriteRaw(uint32_t size)
{
    return getFreeSize() >= size;
}

bool AmpedUp::CircularQueue::canWriteNewMessageData(RemoteMessageSize_t fragmentMessageSize)
{
    return canWriteRaw(fragmentMessageSize + sizeof(RemoteMessageSize_t));
}

bool AmpedUp::CircularQueue::canWriteContinuingMessageData(RemoteMessageSize_t fragmentMessageSize)
{
    return canWriteRaw(fragmentMessageSize);
}

void AmpedUp::CircularQueue::clear()
{
    // Reset used size to 0. May as well reset front index to 0 as well to reduce wrap-arounds, which slightly degrade performance
    xSemaphoreTake(generalLock_, portMAX_DELAY);
    usedSize_ = 0;
    frontIndex_ = 0;
    xSemaphoreGive(generalLock_);
}

void AmpedUp::CircularQueue::print(std::ostream& os)
{
    xSemaphoreTake(generalLock_, portMAX_DELAY);

    uint32_t offset = 0;

    // Loop through the messages in the queue, break out when the end is reached
    // TODO: This could be more elegant
    while(true)
    {
        if (usedSize_ - offset < sizeof(RemoteMessageSize_t))
        {
            // Not enough used bytes left to read a full message size tag
            os << "[incomplete size tag: " << usedSize_ - offset << " bytes]";
            break;
        }

        // Read next message size tag
        RemoteMessageSize_t messageSize;
        peekBuffer(BinaryUtil::asRawData(messageSize), sizeof(RemoteMessageSize_t), offset);
        offset += sizeof(RemoteMessageSize_t);

        if (messageSize == 0)
        {
            os << "[error: empty messages]";
            break;
        }

        if (usedSize_ - offset < messageSize)
        {
            // Not enough used bytes left to read the full message size
            os << "[incomplete message: " << messageSize << " bytes expected, " << usedSize_ - offset << " present]";
            break;
        }

        // Read the message
        os << "[message: " << messageSize << " bytes]";
        offset += messageSize;

        if (usedSize_ - offset == 0)
        {
            // Message was the last thing in the queue, done printing
            break;
        }
        else
        {
            // Add a line break for readability and move on to the next thing in the queue
            os << ",\n";
        }
    }

    // Note: don't add a line break at the end so that std::endl can be used as expected

    xSemaphoreGive(generalLock_);
}

/////////////////////////////////////
//         PRIVATE METHODS         //
//   (no thread synchronization)   //
/////////////////////////////////////


uint32_t AmpedUp::CircularQueue::getEndIndex()
{
    return (frontIndex_ + usedSize_) % BUFFER_SIZE;
}

void AmpedUp::CircularQueue::incrementFront(uint32_t amount)
{
    frontIndex_ = (frontIndex_ + amount) % BUFFER_SIZE;
}

void AmpedUp::CircularQueue::peekBuffer(uint8_t* destination, uint32_t size, uint32_t offset) const
{
    uint32_t startIndex = (frontIndex_ + offset) % BUFFER_SIZE;

    if (BUFFER_SIZE - startIndex < size)
    {
        // If the data to retrieve crosses the end of the buffer, copy it in two halves
        uint32_t firstSegmentSize = BUFFER_SIZE - startIndex;
        uint32_t secondSegmentSize = size - (BUFFER_SIZE - startIndex);
        memcpy(destination, buffer_ + startIndex, firstSegmentSize);
        memcpy(destination + firstSegmentSize, buffer_, secondSegmentSize);
    }
    else
    {
        // If the data to retrieve is already contiguous we can copy it all at once
        memcpy(destination, buffer_ + startIndex, size);
    }
}

void AmpedUp::CircularQueue::readBuffer(uint8_t* destination, uint32_t size, uint32_t offset)
{
    peekBuffer(destination, size, offset);
    popBuffer(size);
}

void AmpedUp::CircularQueue::writeBuffer(const uint8_t* source, uint32_t size)
{
    // Copy data to the end of the used portion of the buffer
    uint32_t startIndex = getEndIndex();

    if (BUFFER_SIZE - startIndex < size)
    {
        // If the data must be written across the end of the buffer, copy it in two halves
        uint32_t firstSegmentSize = BUFFER_SIZE - startIndex;
        uint32_t secondSegmentSize = size - (BUFFER_SIZE - startIndex);
        memcpy(buffer_ + startIndex, source, firstSegmentSize);
        memcpy(buffer_, source + firstSegmentSize, secondSegmentSize);
    }
    else
    {
        // If the data to write can be written contiguously, copy it all at once
        memcpy(buffer_ + startIndex, source, size);
    }
    
    // Push back the end of the used portion of the buffer
    usedSize_ += size;
}

void AmpedUp::CircularQueue::overwriteBufferFront(const uint8_t* source, uint32_t size)
{
    // Copy data to the front of the buffer
    uint32_t startIndex = frontIndex_;

    if (BUFFER_SIZE - startIndex < size)
    {
        // If the data must be written across the end of the buffer, copy it in two halves
        uint32_t firstSegmentSize = BUFFER_SIZE - startIndex;
        uint32_t secondSegmentSize = size - (BUFFER_SIZE - startIndex);
        memcpy(buffer_ + startIndex, source, firstSegmentSize);
        memcpy(buffer_, source + firstSegmentSize, secondSegmentSize);
    }
    else
    {
        // If the data to write can be written contiguously, copy it all at once
        memcpy(buffer_ + startIndex, source, size);
    }
}

void AmpedUp::CircularQueue::popBuffer(uint32_t size)
{
    incrementFront(size);
    usedSize_ -= size;
}


std::ostream& operator<<(std::ostream& os, AmpedUp::CircularQueue& circularQueue)
{
    circularQueue.print(os);
    return os;
}