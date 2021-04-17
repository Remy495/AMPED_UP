
#ifndef _SPI_BUFFER_VIEW_
#define _SPI_BUFFER_VIEW_

#include "BinaryUtil.hxx"

namespace AmpedUp
{
    class SpiBufferView
    {
    public:

        SpiBufferView() = default;

        SpiBufferView(uint8_t* bufferPtr, uint32_t sizeInBytes) : bufferPtr_(bufferPtr), bufferSize_(sizeInBytes)
        {}

        void writeWord(BinaryUtil::uword_t word)
        {
            currentWord() = word;
            currentByteOffset_ += BinaryUtil::BYTES_PER_WORD;
        }

        BinaryUtil::uword_t readWord()
        {
            BinaryUtil::uword_t word = currentWord();
            currentByteOffset_ += BinaryUtil::BYTES_PER_WORD;
            return word;
        }

        uint32_t getCurrentPos() const
        {
            return currentByteOffset_;
        }

        uint32_t getSize() const
        {
            return bufferSize_;
        }

    private:
        uint8_t* bufferPtr_{nullptr};
        uint32_t bufferSize_{};

        uint32_t currentByteOffset_{};

        BinaryUtil::uword_t& currentWord()
        {
            BinaryUtil::uword_t* currentWordPtr = reinterpret_cast<BinaryUtil::uword_t*>(bufferPtr_ + currentByteOffset_);
            return *currentWordPtr;
        }
    };
}

#endif