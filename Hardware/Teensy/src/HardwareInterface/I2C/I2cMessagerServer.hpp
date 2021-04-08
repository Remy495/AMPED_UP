
#ifndef _I2C_MESSAGER_SERVER_
#define _I2C_MESSAGER_SERVER_

#include <array>

#include "I2cInterface.hpp"
#include "I2cConnection.hpp"

#include "Arduino.h"

namespace AmpedUp
{

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Uses I2C in master mode to exchange structured messages with one or more slaves, as well as performing
    ///        error detection and counting.
    ///
    /// @details While running (i.e. from when begin() is called to when end() is called) data is exchanged continuously
    ///          with slaves in turn. I.e. as soon as a transaction with one slave is completed a transaction with the
    ///          next slave is begun. Messages are transmitted and received alternately, except if there is no message
    ///          to transmit then another receive is performed.
    ///
    /// @note This class is a singleton.
    ///
    /// @param[in] HANDLE Which I2C peripheral to use
    /// @param[in] T The datatype for the messages exchanged over I2C
    /// @param[in] SLAVE_COUNT The maximum number of slave to interface with
	///
    template<I2cHandle HANDLE, typename T, uint8_t SLAVE_COUNT>
    class I2cMessager
    {
    public:

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Initiate communication with the slaves
        ///
        /// @param[in] speed The speed of the I2C bus
        /// @param[in] baseAddress The address of the first slave; addresses of other slaves are assumed to be assigned
        ///                        consecutively
	    ///
        void begin(I2cSpeed speed, uint8_t baseAddress = 0)
        {
            // Assign addresses to slaves
            for (uint8_t i = 0; i < SLAVE_COUNT; i++)
            {
                slaveConnections_[i].setAddress(i + baseAddress);
            }

            // Enable the I2C interface
            Interface::enable(speed);
            Interface::setInterruptHandler(AmpedUp::I2cMessager<HANDLE, T, SLAVE_COUNT>::interruptHandler);
            // Interface::getRegisters().MCR |= LPI2C_MCR_MEN;

            // Start the first transaction.
            currentSlaveIndex_ = 0;
            startTransaction();


            // Enable interrupts last so that there are no interruptions while transaction is starting.
            Interface::enableInterrupts();
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Stop communication with the slaves
        ///
        void end()
        {
            Interface::disableInterrupts();
            Interface::setInterruptHandler(nullptr);
            Interface::disable();
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Send a message to a slave
        ///
        /// @param[in] slaveIndex The index of the slave to send the message to. This is not necessarily the same as
        ///                       the slave's I2C address
        /// @param[in] T The message to send
        ///
        /// @note The message will be transmitted at the next oportunity. If a new message is supplied while another
        ///       message is still waiting to be transmitted the old message will be discarged and the new one will be
        ///       transmitted at the next oportunity
        ///
        void sendToIndex(uint8_t slaveIndex, const T& message)
        {
            // Disable interrupts while copying into the outgoing message buffer to avoid split messages.
            Interface::disableInterrupts();
            slaveConnections_[slaveIndex].putOutgoingMessage(message);
            Interface::enableInterrupts();
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Check whether a new message has been received from a particular slave
        ///
        /// @param[in] slaveIndex The index of the slave to check. This is not necessarily the same as the slave's I2C
        ///                       address
        ///
        /// @return true if a new message has been received from the slave in question, otherwise false
        ///
        bool hasReceivedMessageFromIndex(uint8_t slaveIndex)
        {
            return slaveConnections_[slaveIndex].isIncomingMessageFresh();
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Retrieve the message that was most recently received from a particular slave (which is now being held
        ///        in a buffer until taken)
        ///
        /// @param[in] slaveIndex The index of the slave whose message to take. This is not necessarily the same as the
        ///                       slave's I2C address.
        ///
        T takeReceivedMessageFromIndex(uint8_t slaveIndex)
        {
            // Disable interrupts while copying out of the incoming message buffer to avoid split messages.
            Interface::disableInterrupts();
            T tmp = slaveConnections_[slaveIndex].takeIncomingMessage();
            Interface::enableInterrupts();

            return tmp;
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Check whether communication has been established and is ongoing with a particular slave
        ///
        /// @param[in] slaveIndex The index of the slave to check. This is not necessarily the same as the slave's I2C
        ///                       address
        ///
        /// @return True if the slave in question is currently online, otherwise false
        ///
        bool isSlaveOnline(uint8_t slaveIndex)
        {
            return slaveConnections_[slaveIndex].isConnected();
        }
        
        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Manually specify the number of errors that have occured while transmitting messages
        ///
        void setTransmitErrorCountForIndex(uint8_t slaveIndex, uint32_t transmitErrorCount)
        {
            slaveConnections_[slaveIndex].setOutgoingMessageErrorCount(transmitErrorCount);
        }

        uint32_t getMessageCountForIndex(uint8_t slaveIndex)
        {
            uint32_t messageCount = slaveConnections_[slaveIndex].getOutgoingMessageCount() + 
                                    slaveConnections_[slaveIndex].getIncomingMessageCount();
            return messageCount;
        }

        uint32_t getErrorCountForIndex(uint8_t slaveIndex)
        {
            uint32_t messageCount = slaveConnections_[slaveIndex].getOutgoingMessageErrorCount() + 
                                    slaveConnections_[slaveIndex].getIncomingMessageErrorCount();
            return messageCount;
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Get the singleton instance
        ///
        static I2cMessager<HANDLE, T, SLAVE_COUNT>& getInstance()
        {
            static I2cMessager<HANDLE, T, SLAVE_COUNT> instance;
            return instance;
        }

    private:
        using Interface = I2cInterface<HANDLE>;
        using SlaveConnection = I2cConnection<T>;

        // All state is maintained by static variables. This is because the interrupt handler needs to be static so it
        // can be called by the hardware. This class is really only a singleton because calling static functions from
        // application code all the time is a bit ugly

        static inline std::array<SlaveConnection, SLAVE_COUNT> slaveConnections_{};
        static inline volatile uint8_t currentSlaveIndex_{};
        static inline volatile bool currentTransactionSuccess_{true};

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Make the next slave in line the current slave
        ///
        static void advanceCurrentSlaveIndex()
        {
            // Iterate through the slaves in order, then reset back to the beginning
            currentSlaveIndex_ = (currentSlaveIndex_ + 1) % SLAVE_COUNT;
        }

        //////////////////////////////////////////////////////////////////////////////////
	    ///
	    /// @brief Interrupt handler, called by hardware interrupt when an event related to the I2C peripheral occurs
        ///
        static void interruptHandler()
        {

            IMXRT_LPI2C_t& registers = Interface::getRegisters();
            uint32_t msr = registers.MSR;

            // Handler errors

            if (msr & LPI2C_MSR_ALF)
            {
                // Arbitration lost, possibly caused by a wiring problem or a misbehaving slave
                registers.MSR = LPI2C_MSR_ALF;

                // abort current transaction
                abortTransaction(false);
            }

            if (msr & LPI2C_MSR_FEF)
            {
                // FIFO error, possibly indicates a bug somewhere in this file, or another error cascading
                registers.MSR = LPI2C_MSR_FEF;
                if (currentTransactionSuccess_)
                {
                    // abort current transaction
                    abortTransaction(false);
                }
            }

            if (msr & LPI2C_MSR_PLTF)
            {
                // Pin low timeout error, possibly caused by a crashed slave
                registers.MSR = LPI2C_MSR_PLTF;
                if (currentTransactionSuccess_)
                {
                    // abort current transaction; consider the slave that caused it to be offline now.
                    abortTransaction(false);
                }
            }

            if (msr & LPI2C_MSR_NDF)
            {
                // Received NAK. If this occurs on a start condition it means the slave is not online; otherwise it is
                // likely a temporary problem
                registers.MSR = LPI2C_MSR_NDF;
                if (currentTransactionSuccess_)
                {
                    // Hard failure (i.e. disconnect slave) if message has not yet begun (i.e. nak'd start)
                    abortTransaction(!slaveConnections_[currentSlaveIndex_].messageHasBegun());
                }
            }

            // Handle stop condition

            if (msr & LPI2C_MSR_SDF)
            {
                // Stop flag is set immediately after the stop condition is transmitted. This means the transaction has
                // been completed successfully and it's time to move on the next one
                registers.MSR = LPI2C_MSR_SDF;
                if (currentTransactionSuccess_)
                {
                    // Transactions that fail will already have been wrapped up by the error handler condition
                    slaveConnections_[currentSlaveIndex_].endTransaction();
                }
                advanceCurrentSlaveIndex();
                startTransaction();
            }

            // Handle transmit / receive 

            auto& currentConnection = slaveConnections_[currentSlaveIndex_];

            if (msr & LPI2C_MSR_RDF)
            {
                // New data has been received, put it into the incoming message buffer for the current slave
                while(Interface::hasReceivedData())
                {
                    currentConnection.writeIncomingMessageByte(Interface::takeReceivedByte());
                }
            }

            if (currentConnection.isTransmitting() && (msr & LPI2C_MSR_TDF))
            {
                // Outgoing queue is empty, put more data in until it is full
                while(Interface::canTransmitData())
                {
                    if (currentConnection.messageHasFinished())
                    {
                        // Transmission complete, send a stop
                        Interface::sendStop();
                        break;
                    }
                    else
                    {
                        // Send the next byte of the outgoing message
                        Interface::sendByte(currentConnection.readOutgoingMessageByte());
                    }
                }
            }

        }

        static void startTransaction()
        {
            auto& connection = slaveConnections_[currentSlaveIndex_];

            if (!connection.isConnected())
            {
                // If communication with the slave has not yet been established, start a probe message (i.e. a start and
                // then an immediate stop; all we are looking for is whether there was an ack or a nack)
                Interface::sendStart(connection.getAddress(), true);
                Interface::sendStop();
                // Don't start a transaction, as we are just probing for the slave
            }
            else if (connection.isOutgoingMessageFresh() && !connection.wasLastTransactionTransmit())
            {
                // If there is a new message to send and we didn't just send a message, start a transmit. This prevents
                // a situation where we go a long time without receiving messages because we have a lot to send
                connection.startTransaction(true);
                Interface::sendStart(connection.getAddress(), true);
                // Bytes must be written one at a time in the interrupt handler
            }
            else
            {
                // Start a receive transaction
                connection.startTransaction(false);
                Interface::sendStart(connection.getAddress(), false);
                Interface::receiveBytes(SlaveConnection::getMessageSize());
                Interface::sendStop();
            }

            // Each transaction is assumed to be successful until an error occurs
            currentTransactionSuccess_ = true;
        }

        static void abortTransaction(bool hardFailure)
        {

            IMXRT_LPI2C_t& registers = Interface::getRegisters();

            // Discard any received data / outgoing actions
            registers.MCR |= LPI2C_MCR_RTF | LPI2C_MCR_RRF;

            // Abort the transaction in progress with the current slave.
            auto& currentConnection = slaveConnections_[currentSlaveIndex_];
            currentConnection.abortTransaction();
            if (hardFailure)
            {
                // If this was a hard failure, mark the slave as disconnected
                currentConnection.setIsConnected(false);
            }


            uint32_t msr = registers.MSR;
            if (msr & LPI2C_MSR_MBF)
            {
                // If we still have control of the I2C bus, make sure a stop is sent
                if (!(msr & LPI2C_MSR_SDF))
                {
                    Interface::sendStop();
                }
            }
            else
            {
                // If we have lost control of the bus, turn the I2C peripheral off and back on and try to get it back
                registers.MCR &= ~LPI2C_MCR_MEN;
                registers.MCR |= LPI2C_MCR_MEN;

                // Start the next transaction (attempting to regain control of the I2C bus)
                advanceCurrentSlaveIndex();
                startTransaction();
            }

            currentTransactionSuccess_ = false;
        }


    };


}

#endif