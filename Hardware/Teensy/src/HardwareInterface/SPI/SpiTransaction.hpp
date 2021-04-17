

#ifndef _SPI_TRANSACTION_
#define _SPI_TRANSACTION_

#include <iostream>

#include "BinaryUtil.hxx"

#include "Arduino.h"

namespace AmpedUp
{
    class SpiTransaction
    {
    public:

        SpiTransaction() = default;

        SpiTransaction(const BinaryUtil::byte_t* outgoingDataPtr, uint32_t outgoingDataSize, BinaryUtil::byte_t* incomingDataPtr, uint32_t incomingDataSize) :
            outgoingDataPtr_(outgoingDataPtr), outgoingDataSize_(outgoingDataSize), incomingDataPtr_(incomingDataPtr), incomingDataSize_(incomingDataSize)
        {}

        ~SpiTransaction() = default;

        void resetOutgoingData()
        {
            setOutgoingData(nullptr, 0);
        }

        void setOutgoingData(const BinaryUtil::byte_t* outgoingDataPtr, uint32_t outgoingDataSize)
        {
            outgoingDataPtr_ = outgoingDataPtr;
            outgoingDataSize_ = outgoingDataSize;
            bytesSent_ = 0;
        }

        void resetIncomingData()
        {
            setIncomingData(nullptr, 0);
        }

        void setIncomingData(BinaryUtil::byte_t* incomingDataPtr, uint32_t incomingDataSize)
        {
            incomingDataPtr_ = incomingDataPtr;
            incomingDataSize_ = incomingDataSize;
            bytesRecieved_ = 0;
        }

        BinaryUtil::uword_t readOutgoingWord()
        {
            // If there is more data to be sent read it, otherwise return a dummy word
            uint32_t outgoingWord = BLANK_WORD;
            if (bytesSent_ < outgoingDataSize_)
            {
                const BinaryUtil::uword_t* outgoingWordPtr = reinterpret_cast<const BinaryUtil::uword_t*>(outgoingDataPtr_ + bytesSent_);
                outgoingWord = *outgoingWordPtr;
            }
            bytesSent_ += BinaryUtil::BYTES_PER_WORD;
            return outgoingWord;
        }

        void writeIncomingWord(BinaryUtil::uword_t incomingWord)
        {
            if (bytesRecieved_ < incomingDataSize_)
            {
                BinaryUtil::uword_t* incomingWordPtr = reinterpret_cast<BinaryUtil::uword_t*>(incomingDataPtr_ + bytesRecieved_);
                *incomingWordPtr = incomingWord;
            }
            bytesRecieved_ += BinaryUtil::BYTES_PER_WORD;
        }

        bool isFinished() const
        {
            // Why we are comparing bytes sent to incoming data size when it seems like we should be comparing byte
            // recieved: There is a delay between when an outgoing word is given to the SPI hardware and when the
            // incoming word that was transfered at the same time is ready to be recieved. As such, the number of bytes
            // that will eventually be recieved is equal to the number of bytes sent (because SPI is full duplex), but
            // not necessarily equal to the number of bytes that have been recieved now. The transaction is done when
            // we have sent enough bytes and we have committed to recieving enough bytes.
            return bytesSent_ >= outgoingDataSize_ && bytesSent_ >= incomingDataSize_;
        }

        uint32_t getOutgoingSize() const
        {
            return outgoingDataSize_;
        }

        uint32_t getIncomingSize() const
        {
            return incomingDataSize_;
        }
        

    private:

        // Meaningless value to be sent when there is more data to recieve but no more data to send
        static constexpr BinaryUtil::uword_t BLANK_WORD = 0;

        const BinaryUtil::byte_t* outgoingDataPtr_{nullptr};
        uint32_t outgoingDataSize_{};
        
        BinaryUtil::byte_t* incomingDataPtr_{nullptr};
        uint32_t incomingDataSize_{};

        uint32_t bytesSent_{};
        uint32_t bytesRecieved_{};
    };
}

#endif