

#ifndef _SPI_PAYLOAD_
#define _SPI_PAYLOAD_

#include "BinaryUtil.hxx"

namespace AmpedUp
{
    class SpiPayload
    {
    public:

        SpiPayload() = default;

        SpiPayload(BinaryUtil::byte_t* dataPtr, uint16_t size) :  dataPtr_(dataPtr), size_(size)
        {}

        ~SpiPayload() = default;

        BinaryUtil::byte_t* getData(uint16_t offset = 0)
        {
            return dataPtr_ + offset;
        }

        const BinaryUtil::byte_t* getData(uint16_t offset = 0) const
        {
            return dataPtr_ + offset;
        }

        uint16_t getSize() const
        {
            return size_;
        }

    private:
        BinaryUtil::byte_t* dataPtr_{nullptr};
        uint16_t size_{0};
    };
}

#endif