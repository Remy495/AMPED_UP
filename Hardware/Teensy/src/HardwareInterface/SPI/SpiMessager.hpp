
#ifndef _SPI_MESSAGER_
#define _SPI_MESSAGER_

#include "SpiInterface.hpp"
#include "SpiBufferView.hpp"
#include "TypedBuffer.h"
#include "CheckedMessage.h"
#include "RemoteMessageHeader.hpp"

namespace AmpedUp
{

class SpiMessager
{
public:

    static void begin()
    {
        // Set the outgoing message to a default message
        outgoingMessageHeader_ = RemoteMessageHeader(RemoteMessageType::NOTHING_TO_REPORT);

        // Enable the SPI peripheral
        SpiInterface::enable();

        // Enable the necessary interrupts in the interrupt mask register
        // Transfer complete, recieve data high water, transmit data low water
        LPSPI4_IER = LPSPI_IER_TCIE | LPSPI_IER_RDIE;

        SpiInterface::setInterruptHandler(SpiMessager::interruptHandler);
        SpiInterface::enableInterrupts();
    }

    static bool isReadyToSend()
    {
        return false;
    }

    static void beginTransaction()
    {
        SpiInterface::disableInterrupts();

        // Enable the necessary interrupts in the interrupt mask register
        // Transfer complete, recieve data high water, transmit data low water
        LPSPI4_IER = LPSPI_IER_TCIE | LPSPI_IER_RDIE | LPSPI_IER_TDIE;

        status_ = Status::TRANSFERING;

        outgoingData_ = SpiBufferView(outgoingMessageHeader_.getData(), outgoingMessageHeader_.getSize());

        if (incomingMessageRecievedHeader_ && incomingMessageHeader_.getInstance().isValid() && incomingMessageHeader_.getInstance().getPayload().hasPayload())
        {
            incomingData_ = SpiBufferView(incomingMessagePayload_, 12);
            incomingMessageRecievedHeader_ = false;
        }
        else
        {
            incomingData_ = SpiBufferView(incomingMessageHeader_.getData(), incomingMessageHeader_.getSize());
            incomingMessageRecievedHeader_ = true;
        }

        SpiInterface::beginTransaction();

        SpiInterface::enableInterrupts();
    }

    static uint8_t* getIncomingMessagePayload()
    {
        incomingMessagePayload_[11] == '\0';
        return incomingMessagePayload_;
    }

private:

    using MessageHeaderBuffer = WordAlignedTypedBuffer<CheckedMessage<RemoteMessageHeader>>;

    static inline MessageHeaderBuffer outgoingMessageHeader_{};
    static inline MessageHeaderBuffer incomingMessageHeader_{};

    static inline SpiBufferView outgoingData_{};
    static inline SpiBufferView incomingData_{};

    static inline bool outgoingMessageSendHeader_{true};
    static inline bool incomingMessageRecievedHeader_{true};

    // static inline uint8_t* outgoingMessagePayload_{nullptr};
    // static inline uint8_t* incomingMessagePayload_{nullptr};

    static inline uint8_t incomingMessagePayload_[12]{0};



    enum class Status
    {
        IDLE,
        TRANSFERING,
        STOPPING
    };

    static inline volatile Status status_;
    

    static void interruptHandler()
    {
        uint32_t sr = LPSPI4_SR;

        // Respond to transfer complete interrupt
        if (sr & LPSPI_SR_TCF)
        {
            status_ = Status::IDLE;
            // Clear status bit
            LPSPI4_SR = LPSPI_SR_TCF;
        }

        // Respond to transmit request interrupt (if the transaction is still going)
        if (sr & LPSPI_SR_TDF)
        {
            if (status_ == Status::TRANSFERING)
            {
                while (SpiInterface::canTransmitData())
                {
                    if (outgoingData_.dataIsFinished())
                    {
                        SpiInterface::endTransaction();
                        status_ = Status::STOPPING;
                        LPSPI4_IER &= ~LPSPI_IER_TDIE;
                    }
                    else
                    {
                        SpiInterface::sendWord(outgoingData_.readWord());
                    }
                }
            }

            LPSPI4_SR = LPSPI_SR_TDF;
        }

        // Respond to recieve request interrupt
        if (sr & LPSPI_SR_RDF)
        {
            while(SpiInterface::canReceiveData())
            {
                incomingData_.writeWord(SpiInterface::recieveWord());
            }

            LPSPI4_SR = LPSPI_SR_RDF;
        }
    }
    

};

}

#endif