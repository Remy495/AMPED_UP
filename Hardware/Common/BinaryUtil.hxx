
#ifndef _BINARY_UTIL_
#define _BINARY_UTIL_

// IMPORTANT NOTE: Constants in this file assume a 32 bit processor. Currently this is accurate for all the microcontrollers we are using.
// Be sure to make adjustments if this assumption ever stops being true

#include <cstdint>

namespace AmpedUp
{
    class BinaryUtil
    {
    public:

        using word_t = int32_t;
        using uword_t = uint32_t;

        using byte_t = uint8_t;

        static inline constexpr uint32_t BITS_PER_BYTE = 8;
        static inline constexpr uint32_t BYTES_PER_WORD = 4;
        static inline constexpr uint32_t BITS_PER_WORD = BITS_PER_BYTE * BYTES_PER_WORD;

        static constexpr uint32_t bitsFillBytes(uint32_t size)
        {
            return roundUp(size, BITS_PER_BYTE);
        }

        static constexpr uint32_t bytesFillWords(uint32_t size)
        {
            return roundUp(size, BYTES_PER_WORD);
        }

        static constexpr uint32_t bitsFillWords(uint32_t size)
        {
            return roundUp(size, BITS_PER_WORD);
        }

        static constexpr uint32_t bitsToBytes(uint32_t bits)
        {
            return roundUp(bits, BITS_PER_BYTE) / BITS_PER_BYTE;
        }

        static constexpr uint32_t bytesToWords(uint32_t bytes)
        {
            return roundUp(bytes, BYTES_PER_WORD) / BYTES_PER_WORD;
        }

        static constexpr uint32_t bitsToWords(uint32_t bits)
        {
            return roundUp(bits, BITS_PER_WORD) / BITS_PER_WORD;
        }

        static constexpr uint32_t bytesToBits(uint32_t bytes)
        {
            return bytes * BITS_PER_BYTE;
        }

        static constexpr uint32_t wordsToBytes(uint32_t words)
        {
            return words * BYTES_PER_WORD;
        }

        static constexpr uint32_t wordsToBites(uint32_t words)
        {
            return words * BITS_PER_WORD;
        }

        template<typename T>
        static constexpr uint8_t* asRawData(T& item)
        {
            return reinterpret_cast<uint8_t*>(&item);
        }

        template<typename T>
        static constexpr const uint8_t* asRawData(const T& item)
        {
            return reinterpret_cast<const uint8_t*>(&item);
        }

    private:

        static constexpr uint32_t roundUp(uint32_t number, uint32_t multiple)
        {
            return (number + (multiple - 1)) & ~(multiple - 1);
        }
    };
}

#endif