

#ifndef _SPI_PAYLOAD_
#define _SPI_PAYLOAD_

#include "ConstantsCommon.hpp"
#include "BinaryUtil.hxx"
#include "StaticBuffer.hxx"

namespace AmpedUp
{
    class SpiPayload
    {
    public:

        SpiPayload() = default;
        ~SpiPayload() = default;

        BinaryUtil::byte_t* data()
        {
            return payloadBuffer_.data();
        }

        const BinaryUtil::byte_t* data() const
        {
            return payloadBuffer_.data();
        }

        const uint32_t getUsedSize() const
        {
            return usedSize_;
        }

        void setUsedSize(uint32_t size)
        {
            usedSize_ = size;
        }

    private:
        WordAlignedStaticBuffer<Constants::REMOTE_MESSAGE_MAX_SIZE> payloadBuffer_{};
        uint32_t usedSize_{};
    };
}

#endif