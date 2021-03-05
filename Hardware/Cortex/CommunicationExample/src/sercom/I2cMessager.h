


#ifndef I2CSLAVECONTROLLER_H_
#define I2CSLAVECONTROLLER_H_

#include "SercomInterface.h"
#include "I2cConnection.hpp"

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Provides an interface for asynchronously sending and receiving messages over I2C in slave mode
///
template<SercomHandle SERCOM_HANDLE, typename T>
class I2cMessager
{
public:
		
	virtual void begin(uint8_t address, bool useAltPins = false)
	{
		masterConnection_.setAddress(address);
		
		// Enable the sercom unit and take control of pins 1 and 2
		HardwareInterface::enable();
		if (useAltPins)
		{
			HardwareInterface::getAltPins().enablePins(0, 1);
		}
		else
		{
			HardwareInterface::getPins().enablePins(0, 1);
		}
		
		// Set up sercom unit as I2C slave
		Sercom& sercomRegisters = HardwareInterface::getRegisters();
		sercomRegisters.I2CS.CTRLA.reg = SERCOM_I2CS_CTRLA_MODE_I2C_SLAVE;

		// Enable auto-ack
		sercomRegisters.I2CS.CTRLB.reg = SERCOM_I2CS_CTRLB_SMEN;

		// Set slave address
		sercomRegisters.I2CS.ADDR.reg = SERCOM_I2CS_ADDR_ADDR(address);

		// Enable I2C communication when registers are synchronized
		while (sercomRegisters.I2CS.SYNCBUSY.bit.ENABLE);
		sercomRegisters.I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_ENABLE;	

		// Enabled interrupts on address match (i.e. start), stop, data ready, and error
		sercomRegisters.I2CS.INTENSET.reg =
		SERCOM_I2CS_INTENSET_PREC |
		SERCOM_I2CS_INTENSET_AMATCH |
		SERCOM_I2CS_INTENSET_DRDY |
		SERCOM_I2CS_INTENSET_ERROR;

		// Enable interrupts
		HardwareInterface::setInterruptHandler(I2cMessager<SERCOM_HANDLE, T>::processInterrupt);
		HardwareInterface::enableInterrupts();
	}
	
	virtual void end()
	{
		// Disable interrupts
		HardwareInterface::setInterruptHandler(nullptr);
		HardwareInterface::disableInterrupts();
		// Disable the sercom unit
		HardwareInterface::disable();
	}
	
	void setReply(const T& message)
	{
		HardwareInterface::disableInterrupts();
		masterConnection_.putOutgoingMessage(message);
		HardwareInterface::enableInterrupts();
	}
	
	bool hasReceivedMessage() const
	{
		return masterConnection_.isIncomingMessageFresh();
	}
	
	T takeReceivedMessage()
	{
		HardwareInterface::disableInterrupts();
		T receivedMessage = masterConnection_.takeIncomingMessage();;
		HardwareInterface::enableInterrupts();
		
		return receivedMessage;
	}
	
	static I2cMessager<SERCOM_HANDLE, T>& getInstance()
	{
		static I2cMessager<SERCOM_HANDLE, T> instance;
		return instance;
	}

private:

	using HardwareInterface = SercomInterface<SERCOM_HANDLE>;
	using MessageBuffer = TypedDoubleBuffer<T>;
	
	static inline I2cConnection<T> masterConnection_{};
	static inline bool currentTransactionSuccess_{true};

	I2cMessager() = default;
		
	static void processInterrupt()
	{
		// Create a flag variable to be masked with the different possible sources of interrupt (address match, data ready or STOP)
		Sercom& registers = HardwareInterface::getRegisters();
		uint32_t flags = registers.I2CS.INTFLAG.reg;

		// Process error interrupt
		if (flags & SERCOM_I2CS_INTFLAG_ERROR)
		{
			// Abort the current transaction, consider it a failure
			masterConnection_.abortTransaction();
			currentTransactionSuccess_ = false;

			// Clear the error interrupt flag
			registers.I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_ERROR;
		}				
				
		// Process stop interrupt
		if (flags & SERCOM_I2CS_INTFLAG_PREC)
		{
			if (currentTransactionSuccess_)
			{
				// If no errors occured during transaction, end it normally.
				masterConnection_.endTransaction();
			}	
			// Clear stop interrupt flag
			registers.I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
		}

		// Process address match (ie. start)
		if (flags & SERCOM_I2CS_INTFLAG_AMATCH)
		{
			// Start a transaction in the appropriate direction based on the status register
			masterConnection_.startTransaction(registers.I2CS.STATUS.reg & SERCOM_I2CS_STATUS_DIR);
			currentTransactionSuccess_ = true;
					
			// Clear Interrupt Flag Address Match Condition
			registers.I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
		}

		// Process data ready (i.e. request to transmit or receive the next byte)
		if (flags & SERCOM_I2CS_INTFLAG_DRDY)
		{
			if (masterConnection_.isTransmitting())
			{
				// Send the next byte the the master
						
				uint8_t data = 0;
				// Check if master has issued a nack (which would mean it is ending the transaction)
				// Note that the first byte never has a nack.
				if (!masterConnection_.messageHasBegun() || !registers.I2CS.STATUS.bit.RXNACK)
				{
					// If transmission is continuing, load the next byte into the data register.
					data = masterConnection_.readOutgoingMessageByte();
				}
						
				// Put the byte into the data register to be sent
				registers.I2CS.DATA.reg = data;
			}
			else
			{
				// Receive the next byte from the master
				masterConnection_.writeIncomingMessageByte(registers.I2CS.DATA.reg);
			}
		}
	}
	

};


#endif /* I2CSLAVECONTROLLER_H_ */