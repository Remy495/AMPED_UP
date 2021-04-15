
#ifndef _SPI_MESSAGER_
#define _SPI_MESSAGER_

#include <sstream>

#include "Arduino.h"

#include "SpiInterface.hpp"
#include "SpiTransaction.hpp"
#include "TypedBuffer.h"
#include "RemoteMessageHeader.hpp"
#include "SpiPayloadQueue.hpp"

#include "TextLogging.hxx"

namespace AmpedUp
{

class SpiMessager
{
public:

    static void begin()
    {
        // Clear payload queues
        outgoingMessagePayloads_.reset();
        incomingMessagePayloads_.reset();

        // Set a default outgoing header
        outgoingMessageHeader_ = RemoteMessageHeader();

        // Enable the SPI peripheral
        SpiInterface::enable();

        // Enable the necessary interrupts in the interrupt mask register
        // Transfer complete, recieve data high water, transmit data low water
        LPSPI4_IER = LPSPI_IER_TCIE | LPSPI_IER_RDIE;

        SpiInterface::setInterruptHandler(SpiMessager::interruptHandler);
        SpiInterface::enableInterrupts();

        hardwareStatus_ = HardwareStatus::IDLE;
    }

    static bool isReadyToSend()
    {
        return !outgoingMessagePayloads_.isFull() && !outgoingMessageInProgress_;
    }

    static SpiPayload& stageOutgoingMessage()
    {
        SpiPayload& stagedPayload = outgoingMessagePayloads_.stageEnqueue();
        stagedPayload.setUsedSize(0);
        outgoingMessageInProgress_ = true;
        return stagedPayload;
    }

    static void sendStagedMessage()
    {
        outgoingMessagePayloads_.commitStagedEnqueue();
        outgoingMessageInProgress_ = false;
    }

    static void cancelStagedMessage()
    {
        outgoingMessageInProgress_ = false;
    }

    static bool hasRecievedMessage()
    {
        return !incomingMessagePayloads_.isEmpty();
    }

    static const SpiPayload& peekRecievedMessage()
    {
        return incomingMessagePayloads_.peekFront();
    }

    static void discardRecievedMessage()
    {
        incomingMessagePayloads_.dequeue();
    }

    static bool remoteHasConnected()
    {
        bool temp = hasNewConnection_;
        hasNewConnection_ = false;
        return temp;
    }

    static bool remoteHasDisconnected()
    {
        bool temp = hasNewDisconnection_;
        hasNewDisconnection_ = false;
        return temp;
    }

    static void beginTransaction()
    {
        // Only begin a transaction if the SPI messager is initialized but not active
        if (hardwareStatus_ == HardwareStatus::IDLE)
        {

            SpiInterface::disableInterrupts();

            if (sendHeader_)
            {
                // This transfer will exchange message headers
                // Prepare the outgoing message header

                RemoteMessageHeader& outgoingHeader = outgoingMessageHeader_.getInstance();
                outgoingHeader.clearFlags();

                if(outgoingMessagePayloads_.isEmpty())
                {
                    // No outgoing messages to send, thus header has no acompanying payload
                    outgoingHeader.setTotalPayloadSize(0);
                    outgoingHeader.setFragmentPayloadSize(0);
                }
                else
                {
                    // Total size is the size of the next message to send, fragment size is either the amount of message that
                    // has yet to be sent or the maximum size of a fragmnet
                    RemoteMessageSize_t totalOutgoingSize = outgoingMessagePayloads_.peekFront().getUsedSize();
                    RemoteMessageSize_t remainingOutgoingSize = totalOutgoingSize - outgoingPayloadBytesSent_;

                    outgoingHeader.setTotalPayloadSize(totalOutgoingSize);
                    outgoingHeader.setFragmentPayloadSize(std::min(remainingOutgoingSize, Constants::REMOTE_MESSAGE_MTU));

                    // If this is the first fragment of this message, set the first fragment flag
                    if (outgoingPayloadBytesSent_ == 0)
                    {
                        outgoingHeader.setFlag(RemoteMessageFlag::FIRST_FRAGMENT);
                    }

                    // If this is the last fragment of this message, set the last fragment flag
                    if (outgoingPayloadBytesSent_ + outgoingHeader.getFragmentPayloadSize() >= totalOutgoingSize)
                    {
                        outgoingHeader.setFlag(RemoteMessageFlag::LAST_FRAGMENT);
                    
                    }
                }

                // If the incoming payload queue is not full, we are ready to recieve a payload, so set the ready to recieve flag
                if (!incomingMessagePayloads_.isFull())
                {
                    outgoingHeader.setFlag(RemoteMessageFlag::READY_TO_RECIEVE);
                }

                // Make sure the header already present in the incoming buffer is invalid to ensure we don't mistake an interrupted transaction as successful
                RemoteMessageHeader& incomingHeader = incomingMessageHeader_.getInstance();
                incomingHeader.invalidate();

                // Set the headers as the data to transfer
                currentTransaction_ = SpiTransaction(outgoingMessageHeader_.getData(), outgoingMessageHeader_.getSize(), incomingMessageHeader_.getData(), incomingMessageHeader_.getSize());
            }
            else
            {
                // This transfer will exchange message payloads

                RemoteMessageHeader& outgoingHeader = outgoingMessageHeader_.getInstance();
                RemoteMessageHeader& incomingHeader = incomingMessageHeader_.getInstance();

                if (outgoingHeader.hasPayload() && incomingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
                {
                    // Set the appropriate section of the outgoing payload as the data to send
                    SpiPayload& outgoingPayload = outgoingMessagePayloads_.peekFront();
                    currentTransaction_.setOutgoingData(outgoingPayload.data() + outgoingPayloadBytesSent_, outgoingHeader.getFragmentPayloadSize());
                }
                else
                {
                    // No outgoing payload or other side is not ready to recieve; no data will be sent
                    currentTransaction_.resetOutgoingData();
                }

                if (incomingHeader.hasPayload() && outgoingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
                {
                    // Set the appropriate section of the incoming payload as the data to recieve
                    SpiPayload& incomingPayload = incomingMessagePayloads_.stageEnqueue();
                    incomingPayload.setUsedSize(incomingHeader.getTotalPayloadSize());
                    currentTransaction_.setIncomingData(incomingPayload.data() + incomingPayloadBytesRecieved_, incomingHeader.getFragmentPayloadSize());
                }
                else
                {
                    // No incoming payload or we are not ready to recieve; no data will be recieved
                    currentTransaction_.resetIncomingData();
                }
            }

            hardwareStatus_ = HardwareStatus::TRANSFERING;

            // Enable the necessary interrupts in the interrupt mask register
            // Transfer complete, recieve data high water, transmit data low water
            LPSPI4_IER = LPSPI_IER_TCIE | LPSPI_IER_RDIE | LPSPI_IER_TDIE;

            SpiInterface::beginTransaction();

            SpiInterface::enableInterrupts();

        }
    }

private:

    using MessageHeaderBuffer = WordAlignedTypedBuffer<RemoteMessageHeader>;

    enum class HardwareStatus
    {
        UNINITIALIZED,
        IDLE,
        TRANSFERING,
        STOPPING
    };

    // Headers for the segment currently being sent
    static inline MessageHeaderBuffer outgoingMessageHeader_{};
    static inline MessageHeaderBuffer incomingMessageHeader_{};

    // Queues of incoming and outgoing message payloads
    // These large buffers should be kept out of TCM in order to leave room for more time-sensitive variables
    static inline __attribute__ ((section(".dmabuffers"), used)) SpiPayloadQueue<2> outgoingMessagePayloads_{};
    static inline __attribute__ ((section(".dmabuffers"), used)) SpiPayloadQueue<2> incomingMessagePayloads_{};

    static inline SpiTransaction currentTransaction_{};

    // Keep track of the total number of incoming / outgoing bytes of the current messages (regardless of segmentation)
    static inline volatile uint32_t outgoingPayloadBytesSent_{};
    static inline volatile uint32_t incomingPayloadBytesRecieved_{};

    static inline volatile bool hasNewConnection_{};
    static inline volatile bool hasNewDisconnection_{};
    static inline volatile bool outgoingMessageInProgress_{};

    // Whether the current transfer is message headers or message payloads
    static inline volatile bool sendHeader_{true};

    // Current transaction status
    static inline volatile HardwareStatus hardwareStatus_{HardwareStatus::UNINITIALIZED};
    static inline volatile bool keepIncomingMessage_{true};

    static void finishTransaction()
    {
        if (sendHeader_)
        {
            // Decide whether the next transaction should be a header or a payload
            RemoteMessageHeader& outgoingHeader = outgoingMessageHeader_.getInstance();
            RemoteMessageHeader& incomingHeader = incomingMessageHeader_.getInstance();

            TextLogging::info(__FILE__, __LINE__, "recieved header: ", incomingHeader);

            if (incomingHeader.isValid() && incomingHeader.hasPayload() && outgoingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
            {
                sendHeader_ = false;

                 // If this transaction is flagged as the first fragment but shouldn't be, discard the current message
                 // in progress and start a new one
                 if (incomingHeader.hasFlag(RemoteMessageFlag::FIRST_FRAGMENT) && incomingPayloadBytesRecieved_ > 0)
                 {
                     incomingPayloadBytesRecieved_ = 0;
                     TextLogging::warning(__FILE__, __LINE__, "New remote message started before ongoing message was terminated");
                 }

                 // Similarly, If this transaction should be the first fragment but isn't flaged as such, discard the message
                 if (incomingPayloadBytesRecieved_ == 0)
                 {
                     keepIncomingMessage_ = true;
                     if (!incomingHeader.hasFlag(RemoteMessageFlag::FIRST_FRAGMENT))
                     {
                        keepIncomingMessage_ = false;
                        TextLogging::warning(__FILE__, __LINE__, "Remote message was terminated but new message was not started as expected");
                     }
                 }
            }

            if (incomingHeader.isValid() && outgoingHeader.hasPayload() && incomingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
            {
                // Transfer payload if at least one side has a payload to send and at least the other side is ready to recieve it
                sendHeader_ = false;
            }

            TextLogging::info(__FILE__, __LINE__, "Sent header: ", outgoingHeader);

        }
        else
        {
            // Update the sent byte count for the outgoing message and, if it has been completed, dequeue it
            outgoingPayloadBytesSent_ += currentTransaction_.getOutgoingSize();
            RemoteMessageSize_t outgoingPayloadSize = outgoingMessagePayloads_.peekFront().getUsedSize();
            if (outgoingPayloadBytesSent_ >= outgoingPayloadSize)
            {
                outgoingMessagePayloads_.dequeue();
                outgoingPayloadBytesSent_ = 0;
            }

            // Update recieved byte count for the incoming message and, if it has been completed finish enqueuing it
            incomingPayloadBytesRecieved_ += currentTransaction_.getIncomingSize();
            RemoteMessageSize_t incomingPayloadSize = incomingMessagePayloads_.stageEnqueue().getUsedSize();

            RemoteMessageHeader& incomingHeader = incomingMessageHeader_.getInstance();
            if (incomingPayloadBytesRecieved_ >= incomingPayloadSize || incomingHeader.hasFlag(RemoteMessageFlag::LAST_FRAGMENT))
            {
                // At the end of the message we should have recieved the expected number of bytes and we should have the last fragment flag.
                // If either condition is false, discard the message
                if (incomingPayloadBytesRecieved_ < incomingPayloadSize || !incomingHeader.hasFlag(RemoteMessageFlag::LAST_FRAGMENT))
                {
                    keepIncomingMessage_ = false;
                    TextLogging::warning(__FILE__, __LINE__, "Remote message was not terminated properly");
                }

                // Enqueue the recieved message if it should be kept, otherwise overwrite it next transfer
                if (keepIncomingMessage_)
                {
                    incomingMessagePayloads_.commitStagedEnqueue();
                }
                incomingPayloadBytesRecieved_ = 0;
            }

            // Always send payload after sending header
            sendHeader_ = true;
        }

        hardwareStatus_ = HardwareStatus::IDLE;
    }

    static void interruptHandler()
    {
        uint32_t sr = LPSPI4_SR;

        // Respond to recieve request interrupt
        if (sr & LPSPI_SR_RDF)
        {
            while(SpiInterface::canReceiveData())
            {
                currentTransaction_.writeIncomingWord(SpiInterface::recieveWord());
            }

            LPSPI4_SR = LPSPI_SR_RDF;
        }

        // Respond to transfer complete interrupt
        if (sr & LPSPI_SR_TCF)
        {
            finishTransaction();
            // Clear status bit
            LPSPI4_SR = LPSPI_SR_TCF;
        }

        // Respond to transmit request interrupt (if the transaction is still going)
        if (sr & LPSPI_SR_TDF)
        {
            if (hardwareStatus_ == HardwareStatus::TRANSFERING)
            {
                while (SpiInterface::canTransmitData())
                {
                    if (currentTransaction_.isFinished())
                    {
                        // No more data to transfer, end the transaction (once all of the data has been transfered out of the queue)
                        SpiInterface::endTransaction();
                        hardwareStatus_ = HardwareStatus::STOPPING;
                        LPSPI4_IER &= ~LPSPI_IER_TDIE;
                        break; 
                    }
                    else
                    {
                        SpiInterface::sendWord(currentTransaction_.readOutgoingWord());
                    }
                }
            }

            LPSPI4_SR = LPSPI_SR_TDF;
        }
    }
    

};

}

#endif