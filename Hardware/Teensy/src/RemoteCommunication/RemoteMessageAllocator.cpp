

#include "TextLogging.hxx"

#include "RemoteMessageAllocator.hpp"
#include "SpiMessager.hpp"

AmpedUp::RemoteMessageAllocator::RemoteMessageAllocator() = default;

AmpedUp::RemoteMessageAllocator::~RemoteMessageAllocator() = default;

AmpedUp::BinaryUtil::byte_t* AmpedUp::RemoteMessageAllocator::allocate(size_t size)
{
    SpiPayload payloadBuffer = SpiMessager::stageOutgoingMessage(size);
    return payloadBuffer.getData();
}

void AmpedUp::RemoteMessageAllocator::deallocate(BinaryUtil::byte_t* ptr, size_t size)
{
    SpiMessager::cancelStagedMessage();
}

AmpedUp::BinaryUtil::byte_t* AmpedUp::RemoteMessageAllocator::reallocate_downward(BinaryUtil::byte_t* oldPtr, size_t oldSize, size_t newSize, size_t tailUsed, size_t headUsed)
{
    SpiPayload payloadBuffer = SpiMessager::resizeStagedOutgoingMessage(newSize, headUsed, tailUsed);
    return payloadBuffer.getData();
}