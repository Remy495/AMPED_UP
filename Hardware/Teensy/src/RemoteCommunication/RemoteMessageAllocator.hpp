

#ifndef _REMOTE_MESSAGE_ALLOCATOR_
#define _REMOTE_MESSAGE_ALLOCATOR_

#include "flatbuffers/flatbuffers.h"

#include "BinaryUtil.hxx"

namespace AmpedUp
{
    class RemoteMessageAllocator : public flatbuffers::Allocator
    {
    public:
        RemoteMessageAllocator();

        ~RemoteMessageAllocator();
        
        BinaryUtil::byte_t* allocate(size_t size) override;

        void deallocate(BinaryUtil::byte_t* ptr, size_t size) override;

        BinaryUtil::byte_t* reallocate_downward(BinaryUtil::byte_t* oldPtr, size_t oldSize, size_t newSize, size_t tailUsed, size_t headUsed) override;
    private:
    };

}

#endif