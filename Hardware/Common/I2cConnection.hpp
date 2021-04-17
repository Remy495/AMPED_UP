

#ifndef _I2C_CONNECTION_
#define _I2C_CONNECTION_

#include "TypedDoubleBuffer.h"
#include "CheckedMessage.h"

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Maintains the state machine and the incoming and outgoing messages for an I2C connection (i.e. an ongoing
///        exchange of messages between one master and one slave)
///
/// @note This class is appropriate for use both by I2C masters and slaves
///
template<typename T>
class I2cConnection
{
public:

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a new I2C connection
	///
    constexpr I2cConnection() = default;

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
    ~I2cConnection() = default;

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Set the address of the slave party of this connection
    ///
    /// @param[in] address the address of the slave
	///
    constexpr void setAddress(uint8_t address)
    {
        address_ = address;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the address of the slave party of this connection
	///
    constexpr uint8_t getAddress() const
    {
        return address_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether or not this connection has an outgoing message waiting to be sent
	///
    constexpr bool isOutgoingMessageFresh() const
    {
        return outgoingMessageIsFresh_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether or not this connection has a valid incoming message waiting to be passed along to the
    ///        application
	///
    constexpr bool isIncomingMessageFresh() const
    {
        return incomingMessageIsFresh_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether this connection is currently in the middle of receiving a message
	///
    constexpr bool isReceiving() const
    {
        return status_ == Status::RECEIVING;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether this connection is currently in the middle of transmitting a message
	///
    constexpr bool isTransmitting() const
    {
        return status_ == Status::TRANSMITTING;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether this connection is currently idle (i.e. not transmitting or receiving).
	///
    constexpr bool isIdle() const
    {
        return status_ == Status::DISCONNECTED || status_ == Status::CONNECTED_IDLE;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether contact has been established with the other party of this connection
	///
    constexpr bool isConnected() const
    {
        return status_ != Status::DISCONNECTED;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Specify whether or not the other party of this connection has connected
	///
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

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Start a transaction with the other party
    ///
    /// @param[in] isTransmit Whether this transaction will consist of transmitting a message to the other party (true)
    ///                       or receiving a message from the other party (false)
	///
    constexpr void startTransaction(bool isTransmit)
    {
        if (isTransmit)
        {
            status_ = Status::TRANSMITTING;
                    
            // If this is a transmit transaction, check if a new message has been written to the outgoing message
            // buffer and, if so, swap it to the read side (so it can be sent). If no new outgoing message is present
            // we will just re-send the previous message
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

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief End the currently open transaction with the other party
	///
    constexpr void endTransaction()
    {
        // If this was a receive transaction, check whether the received message was valid. If it was, swap it to the
        // read buffer so it can be taken by the application.
        if (status_ == Status::RECEIVING)
        {
            if (messageHasFinished() && incomingMessage_.getWriterBuffer().getInstance().isValid())
            {
                // Message is valid, pass it along to the application
                incomingMessage_.swapBuffers();
                incomingMessageIsFresh_ = true;
            }
            else
            {
                // Message is invalid, increment the count of incoming message errors
                incomingMessageErrorCount_++;
            }

            incomingMessageCount_++;
            lastTransactionWasTransmit_ = false;
        }
        else if (status_ == Status::TRANSMITTING)
        {
            lastTransactionWasTransmit_ = true;
        }
        status_ = Status::CONNECTED_IDLE;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Abort the currently open transaction with the other party (in response to a protocol error)
    ///
    /// @note After an aborted transaction the other party is still assumed to be connected unless it is manually set as
    ///       disconnected later
	///
    constexpr void abortTransaction()
    {
        // Basically the same as endTransaction(), but treat any data that was received during the transaction as
        // garbage (i.e. don't swap the incoming message buffer, increment the error counter)

        if (isConnected())
        {
            // An aborted message should only be counted as an error if the other party was online.

            if (status_ == Status::RECEIVING)
            {
                lastTransactionWasTransmit_ = false;
                incomingMessageErrorCount_++;
                incomingMessageCount_++;
            }
            else if (status_ == Status::TRANSMITTING)
            {
                outgoingMessageErrorCount_++;
                outgoingMessageCount_++;
                lastTransactionWasTransmit_ = true;
            }

            status_ = Status::CONNECTED_IDLE;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Send an outgoing message to the other party at the next oportunity
    ///
    /// @param[in] message The payload of the message to send
    ///
    /// @note There is no queue of outgoing messages; calling this method when there is already a message waiting to be
    ///       sent replaces the existing message
	///
    constexpr void putOutgoingMessage(const T& message)
    {
        outgoingMessageIsFresh_ = true;
        outgoingMessage_.getWriterBuffer() = message;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Take the message that was most recently received from the other party
    ///
    /// @note This is different from a normal getter because it also clears the 'new message received' flag.
    /// @note There is no queue of incoming messages; If multiple messages have been received from the other party then
    ///       only the most recent one can be retrieved
	///
    constexpr T takeIncomingMessage()
    {
        incomingMessageIsFresh_ = false;
        return incomingMessage_.getReaderBuffer().getInstance().getPayload();
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get a particular byte of the current outgoing message
    ///
    /// @param[in] pos The offset (in bytes) within the message of the byte to retrieve
    ///
    /// @return The value of the requested byte in the message
	///
    constexpr uint8_t getOutgoingMessageByte(uint32_t pos)
    {
        return outgoingMessage_.getReaderBuffer().getData()[pos];
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief During a transmit transaction, retrieve the next byte of the outgoing message. Successive calls to this
    ///        function will read the entire message byte by byte
    ///
    /// @return the next byte of the outgoing message
	///
    constexpr uint8_t readOutgoingMessageByte()
    {
        uint8_t messageByte = getOutgoingMessageByte(currentByteOffset_ % getMessageSize());
        currentByteOffset_++;
        return messageByte;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Set a particular byte in the current incoming message
    ///
    /// @param[in] pos The offset (in bytes) within the message of the byte to set
    /// @param[in] value The value to assign that byte
	///
    constexpr void setIncomingMessageByte(uint32_t pos, uint8_t value)
    {
        incomingMessage_.getWriterBuffer().getData()[pos] = value;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief During a receive transaction, assign the next byte of the incoming message. Successive calls to this
    ///        function will write the entire message byte by byte
    ///
    /// @param[in] value The value to assign the next byte
	///
    constexpr void writeIncomingMessageByte(uint8_t value)
    {
        setIncomingMessageByte(currentByteOffset_ % getMessageSize(), value);
        currentByteOffset_++;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief During a transaction, check whether reading / writing the message has begun
	///
    constexpr bool messageHasBegun() const
    {
        return currentByteOffset_ > 0;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief During a transaction, check whether reading / writing the message has been completed (i.e. has reached
    ///        the end of the message)
	///
    constexpr bool messageHasFinished() const
    {
        return currentByteOffset_ >= getMessageSize();
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether the most recently completed transaction was a transmit or a receive
	///
    constexpr bool wasLastTransactionTransmit() const
    {
        return lastTransactionWasTransmit_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Specify the number of messages sent by this party experienced errors
    ///
    /// @note The reason this function needs to exist is because many kinds of errors (such as bit errors) can only be
    ///       identified by the receiver of the message. Thus external information is needed in order to maintain an
    ///       accurate count.
	///
    constexpr void setOutgoingMessageErrorCount(uint32_t errorCount)
    {
        outgoingMessageErrorCount_ = errorCount;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the number of messages that have been transmitted to the other party (successfully or not)
	///
    constexpr uint32_t getOutgoingMessageCount() const
    {
        return outgoingMessageCount_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the number of messages that failed to be transmitted to the other party. May be an undercount if
    ///        setOutgoingMessageErrorCount() is not 
	///
    constexpr uint32_t getOutgoingMessageErrorCount() const
    {
        return outgoingMessageErrorCount_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether the most recently completed transaction was a transmit or a receive
	///
    constexpr uint32_t getIncomingMessageCount() const
    {
        return incomingMessageCount_;
    }

    constexpr uint32_t getIncomingMessageErrorCount() const
    {
        return incomingMessageErrorCount_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the size of one message. (which must be the same for all messages)
	///
    static constexpr uint32_t getMessageSize()
    {
        return sizeof(Message_t);
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
    AmpedUp::TypedDoubleBuffer<Message_t> outgoingMessage_{};

    volatile bool incomingMessageIsFresh_{false};
    AmpedUp::TypedDoubleBuffer<Message_t> incomingMessage_{};

    volatile Status status_{Status::DISCONNECTED};
    volatile bool lastTransactionWasTransmit_{};
    volatile uint32_t currentByteOffset_{0};

    volatile uint32_t outgoingMessageCount_{};
    volatile uint32_t outgoingMessageErrorCount_{};
    volatile uint32_t incomingMessageCount_{};
    volatile uint32_t incomingMessageErrorCount_{};
};


#endif