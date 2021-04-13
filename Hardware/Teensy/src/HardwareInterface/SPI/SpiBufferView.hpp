
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

        bool dataIsFinished()
        {
            return currentByteOffset_ >= bufferSize_;
        }

    private:
        uint8_t* bufferPtr_{};
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