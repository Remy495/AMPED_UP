

#ifndef _STATIC_BUFFER_
#define _STATIC_BUFFER_

#include "BinaryUtil.hxx"

namespace AmpedUp
{
    template<uint32_t size, bool wordAlign>
    class StaticBuffer_t
    {
    public:

        StaticBuffer_t() = default;

        ~StaticBuffer_t() = default;

        BinaryUtil::byte_t* data()
        {
            return data_;
        }

        const BinaryUtil::byte_t* data() const
        {
            return data_;
        }

        uint32_t getSize() const
        {
            return bufferSize;
        }

    private:

	static constexpr uint32_t bufferAlignment = wordAlign ? alignof(AmpedUp::BinaryUtil::word_t) : alignof(BinaryUtil::byte_t);
	static constexpr uint32_t bufferSize = wordAlign ? AmpedUp::BinaryUtil::bytesFillWords(size) : size;

        alignas(bufferAlignment) BinaryUtil::byte_t data_[bufferSize]{0};
    };

    template<uint32_t size>
    using StaticBuffer = StaticBuffer_t<size, false>;

    template<uint32_t size>
    using WordAlignedStaticBuffer = StaticBuffer_t<size, true>;
}

#endif