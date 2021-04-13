

#ifndef _SPI_BUFFER_
#define _SPI_BUFFER_

#include <cstdint>
#include <new>

#include "BinaryUtil.hxx"

namespace AmpedUp
{
    class SpiBuffer
    {
    public:
        SpiBuffer() = default;

        bool allocate(uint32_t size)
        {
            bool status = true;

            // Make sure buffer is padded to the next word boundary
            size_ = BinaryUtil::bytesFillWords(size);
            data_ = new (std::nothrow) uint8_t[size];
            if (data_ == nullptr)
            {
                size_ = 0;
                status = false;
            }

            return status;
        }

        bool isUsed()
        {
            return data_ != nullptr;
        }

    private:
        uint8_t* data_{nullptr};
        uint32_t size_{0};
    };
}

#endif