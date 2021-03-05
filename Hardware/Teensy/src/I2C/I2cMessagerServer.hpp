
#ifndef _I2C_MESSAGER_SERVER_
#define _I2C_MESSAGER_SERVER_

#include <array>

#include "I2cInterface.hpp"
#include "I2cConnection.hpp"

#include "Arduino.h"

namespace AmpedUp
{

    template<I2cHandle HANDLE, typename T, uint8_t SLAVE_COUNT>
    class I2cMessager
    {
    public:

        void begin(I2cSpeed speed, uint8_t baseAddress = 0)
        {
            // Assign addresses to slaves
            for (uint8_t i = 0; i < SLAVE_COUNT; i++)
            {
                slaveConnections_[i].setAddress(i + baseAddress);
            }

            // // Enable the I2C interface
            Interface::enable();
            Interface::setSpeed(speed);
            Interface::setInterruptHandler(AmpedUp::I2cMessager<HANDLE, T, SLAVE_COUNT>::interruptHandler);

            // Start the first transaction.
            currentSlaveIndex_ = 0;
            startTransaction();

            Interface::getRegisters().MCR |= LPI2C_MCR_MEN;

            // Enable interrupts last so that there are no interruptions while transaction is starting.
            Interface::enableInterrupts();
        }

        void end()
        {
            Interface::disableInterrupts();
            Interface::setInterruptHandler(nullptr);
            Interface::disable();
        }

        void sendToIndex(uint8_t slaveIndex, const T& message)
        {
            // Disable interrupts while copying into the outgoing message buffer to avoid split messages.
            Interface::disableInterrupts();
            slaveConnections_[slaveIndex].putOutgoingMessage(message);
            Interface::enableInterrupts();
        }

        bool hasReceivedMessageFromIndex(uint8_t slaveIndex)
        {
            return slaveConnections_[slaveIndex].isIncomingMessageFresh();
        }

        T takeReceivedMessageFromIndex(uint8_t slaveIndex)
        {
            // Disable interrupts while copying out of the incoming message buffer to avoid split messages.
            Interface::disableInterrupts();
            T tmp = slaveConnections_[slaveIndex].takeIncomingMessage();
            Interface::enableInterrupts();

            return tmp;
        }

        static I2cMessager<HANDLE, T, SLAVE_COUNT>& getInstance()
        {
            static I2cMessager<HANDLE, T, SLAVE_COUNT> instance;
            return instance;
        }

    private:
        using Interface = I2cInterface<HANDLE>;

        static inline std::array<I2cConnection<T>, SLAVE_COUNT> slaveConnections_{};
        static inline volatile uint8_t currentSlaveIndex_{};
        static inline volatile bool currentTransactionSuccess_{true};

        static void advanceCurrentSlaveIndex()
        {
            currentSlaveIndex_ = (currentSlaveIndex_ + 1) % SLAVE_COUNT;
        }

        static void log_master_status_register(uint32_t msr) {
            if (msr) {
                Serial.print("MSR Flags: ");
            }
            if (msr & LPI2C_MSR_BBF) {
                Serial.print("BBF ");
            }
            if (msr & LPI2C_MSR_MBF) {
                Serial.print("MBF ");
            }
            if (msr & LPI2C_MSR_DMF) {
                Serial.print("DMF ");
            }
            if (msr & LPI2C_MSR_PLTF) {
                Serial.print("PLTF ");
            }
            if (msr & LPI2C_MSR_FEF) {
                Serial.print("FEF ");
            }
            if (msr & LPI2C_MSR_ALF) {
                Serial.print("ALF ");
            }
            if (msr & LPI2C_MSR_NDF) {
                Serial.print("NDF ");
            }
            if (msr & LPI2C_MSR_SDF) {
                Serial.print("SDF ");
            }
            if (msr & LPI2C_MSR_EPF) {
                Serial.print("EPF ");
            }
            if (msr & LPI2C_MSR_RDF) {
                Serial.print("RDF ");
            }
            if (msr & LPI2C_MSR_TDF) {
                Serial.print("TDF ");
            }
            if (msr) {
                Serial.println();
            }
        }

        static void interruptHandler()
        {

            IMXRT_LPI2C_t& registers = Interface::getRegisters();
            uint32_t msr = registers.MSR;

            // log_master_status_register(msr);

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
                        // Serial.println("Sending stop");
                        // Transmission complete, send a stop
                        Interface::sendStop();
                        break;
                    }
                    else
                    {
                        // Serial.println("Sending byte");
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
                // Serial.println("Starting probe message");
                // If communication with the slave has not yet been established, start a probe message (i.e. a start and
                // then an immediate stop; all we are looking for is whether there was an ack or a nack)
                Interface::sendStart(connection.getAddress(), true);
                Interface::sendStop();
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
                Interface::receiveBytes(connection.getMessageSize());
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