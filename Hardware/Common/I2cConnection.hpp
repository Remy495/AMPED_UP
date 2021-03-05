

#ifndef _I2C_CONNECTION_
#define _I2C_CONNECTION_

#include "TypedDoubleBuffer.h"
#include "CheckedMessage.h"

template<typename T>
class I2cConnection
{
public:

    constexpr I2cConnection() = default;

    ~I2cConnection() = default;

    constexpr void setAddress(uint8_t address)
    {
        address_ = address;
    }

    constexpr uint8_t getAddress() const
    {
        return address_;
    }

    constexpr bool isOutgoingMessageFresh() const
    {
        return outgoingMessageIsFresh_;
    }

    constexpr bool isIncomingMessageFresh() const
    {
        return incomingMessageIsFresh_;
    }

    constexpr bool isReceiving() const
    {
        return status_ == Status::RECEIVING;
    }

    constexpr bool isTransmitting() const
    {
        return status_ == Status::TRANSMITTING;
    }

    constexpr bool isIdle() const
    {
        return status_ == Status::DISCONNECTED || status_ == Status::CONNECTED_IDLE;
    }

    constexpr bool isConnected() const
    {
        return status_ != Status::DISCONNECTED;
    }

    constexpr void setIsConnected(bool isConnected)
    {
        if (!isConnected)
        {
            status_ = Status::DISCONNECTED;
        }
        else if (status_ == Status::DISCONNECTED)
        {
            status_ = Status::CONNECTED_IDLE;
        }
    }

    constexpr void startTransaction(bool isTransmit)
    {
        if (isTransmit)
        {
            status_ = Status::TRANSMITTING;
                    
            // If this is a transmit transaction, check if a new message has been written to the outgoing message
            // buffer and, if so, swap it to the read side. (so it can be sent)
            if (outgoingMessageIsFresh_)
            {
                outgoingMessage_.swapBuffers();
                outgoingMessageIsFresh_ = false;
            }
        }
        else
        {
            status_ = Status::RECEIVING;
        }

        // Start transaction at the start of a message
        currentByteOffset_ = 0;
    }

    constexpr void endTransaction()
    {
        // If this was a receive transaction, move the newly received message over to the read side of the buffer
        // so it can be retrieved by the application
        if (status_ == Status::RECEIVING)
        {
            if (incomingMessage_.getWriterBuffer().getInstance().isValid())
            {
                // Check that the received message is valid (i.e. has no bit errors) before giving it to the application
                incomingMessage_.swapBuffers();
                incomingMessageIsFresh_ = true;
            }
            lastTransactionWasTransmit_ = false;
        }
        else if (status_ == Status::TRANSMITTING)
        {
            lastTransactionWasTransmit_ = true;
        }
        status_ = Status::CONNECTED_IDLE;
    }

    constexpr void abortTransaction()
    {
        // Basically the same as endTransaction(), but treat any data that was received during the transaction as
        // garbage (i.e. don't swap the incoming message buffer, overwrite it next receive)

        if (status_ == Status::RECEIVING)
        {
            lastTransactionWasTransmit_ = false;
        }
        else if (status_ == Status::TRANSMITTING)
        {
            lastTransactionWasTransmit_ = true;
        }

        status_ = Status::CONNECTED_IDLE;
    }

    constexpr void putOutgoingMessage(const T& message)
    {
        outgoingMessageIsFresh_ = true;
        outgoingMessage_.getWriterBuffer() = message;
    }

    constexpr T takeIncomingMessage()
    {
        incomingMessageIsFresh_ = false;
        return incomingMessage_.getReaderBuffer().getInstance().getPayload();
    }

    constexpr uint8_t getOutgoingMessageByte(uint32_t pos)
    {
        return outgoingMessage_.getReaderBuffer().getData()[pos];
    }

    constexpr uint8_t readOutgoingMessageByte()
    {
        uint8_t messageByte = getOutgoingMessageByte(currentByteOffset_ % getMessageSize());
        currentByteOffset_++;
        return messageByte;
    }

    constexpr void setIncomingMessageByte(uint32_t pos, uint8_t value)
    {
        incomingMessage_.getWriterBuffer().getData()[pos] = value;
    }

    constexpr void writeIncomingMessageByte(uint8_t value)
    {
        setIncomingMessageByte(currentByteOffset_ % getMessageSize(), value);
        currentByteOffset_++;
    }

    constexpr bool messageHasBegun() const
    {
        return currentByteOffset_ > 0;
    }

    constexpr bool messageHasFinished() const
    {
        return currentByteOffset_ >= getMessageSize();
    }

    constexpr uint32_t getMessageSize() const
    {
        return sizeof(Message_t);
    }

    constexpr bool wasLastTransactionTransmit() const
    {
        return lastTransactionWasTransmit_;
    }

private:

	enum class Status : uint8_t
	{
		DISCONNECTED,   // Connection to partner chip has not been established or has experienced an error
        CONNECTED_IDLE, // Connection to partner chip has been established but is not in use
		TRANSMITTING,   // Currently transmitting data to partner chip
		RECEIVING       // Currently receiving data from partner chip
	};

    using Message_t = CheckedMessage<T>;

    uint8_t address_{};

    volatile bool outgoingMessageIsFresh_{false};
    TypedDoubleBuffer<Message_t> outgoingMessage_{};

    volatile bool incomingMessageIsFresh_{false};
    TypedDoubleBuffer<Message_t> incomingMessage_{};

    volatile Status status_{Status::DISCONNECTED};
    volatile bool lastTransactionWasTransmit_{};
    volatile uint32_t currentByteOffset_{0};
};


#endif