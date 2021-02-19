


#ifndef I2CSLAVECONTROLLER_H_
#define I2CSLAVECONTROLLER_H_

#include "SercomInterface.h"
#include "TypedDoubleBuffer.h"

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
		address_ = address;
		
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
		sercomRegisters.I2CS.ADDR.reg = SERCOM_I2CS_ADDR_ADDR(address_);

		// Enable I2C communication when registers are synchronized
		while (sercomRegisters.I2CS.SYNCBUSY.bit.ENABLE);
		sercomRegisters.I2CS.CTRLA.reg |= SERCOM_I2CS_CTRLA_ENABLE;	

		// Enabled interrupts on address match (i.e. start), stop, and data ready
		sercomRegisters.I2CS.INTENSET.reg =
		SERCOM_I2CS_INTENSET_PREC |
		SERCOM_I2CS_INTENSET_AMATCH |
		SERCOM_I2CS_INTENSET_DRDY;

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
		outgoingMessage_.getWriterBuffer() = message;
		outgoingMessageIsFrech_ = true;
		HardwareInterface::enableInterrupts();
	}
	
	bool hasReceivedMessage() const
	{
		return incomingMessageIsFresh_;
	}
	
	T takeReceivedMessage()
	{
		HardwareInterface::disableInterrupts();
		T receivedMessage(incomingMessage_.getReaderBuffer().getInstance());
		incomingMessageIsFresh_ = false;
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
	
	enum class Status
	{
		IDLE,
		TRANSMITTING,
		RECEIVING
	};

	static inline uint8_t address_{};
	
	static inline MessageBuffer incomingMessage_{};
	static inline volatile bool incomingMessageIsFresh_{false};
	
	static inline MessageBuffer outgoingMessage_{};
	static inline volatile bool outgoingMessageIsFrech_{false};
		
	static inline volatile uint32_t currentTransactionOffset_{0};
		
	static inline volatile Status status_{};

	I2cMessager() = default;
		
	static void processInterrupt()
	{
		// Create a flag variable to be masked with the different possible sources of interrupt (address match, data ready or STOP)
		Sercom& registers = HardwareInterface::getRegisters();
		uint32_t flags = registers.I2CS.INTFLAG.reg;
				
				
		// Process stop interrupt
		if (flags & SERCOM_I2CS_INTFLAG_PREC)
		{
			if (status_ == Status::RECEIVING)
			{
				incomingMessage_.swapBuffers();
				incomingMessageIsFresh_ = true;
			}
					
			// Clear stop interrupt flag
			registers.I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_PREC;
			// Set status to idle
			status_ = Status::IDLE;
		}

		// Process address match (ie. start)
		if (flags & SERCOM_I2CS_INTFLAG_AMATCH)
		{
			// Set the status to transmitting or receiving depending on the data direction flag
			if (registers.I2CS.STATUS.reg & SERCOM_I2CS_STATUS_DIR)
			{
				status_ = Status::TRANSMITTING;
						
				if (outgoingMessageIsFrech_)
				{
					outgoingMessage_.swapBuffers();
					outgoingMessageIsFrech_ = false;
				}
			}
			else
			{
				status_ = Status::RECEIVING;
			}
					
			// Reset message offset
			currentTransactionOffset_ = 0;
					
			// Clear Interrupt Flag Address Match Condition
			registers.I2CS.INTFLAG.reg = SERCOM_I2CS_INTFLAG_AMATCH;
		}

		// Process data ready (i.e. request to transmit or receive the next byte)
		if (flags & SERCOM_I2CS_INTFLAG_DRDY)
		{
			if (status_ == Status::TRANSMITTING)
			{
				// Send the next byte the the master
						
				uint8_t data = 0;
				// Check if master has issued a nack (which would mean it is ending the transaction)
				// Note that the first byte never has a nack.
				if (currentTransactionOffset_ == 0 || !registers.I2CS.STATUS.bit.RXNACK)
				{
					// If transmission is continuing, load the next byte into the data register.
					data = outgoingMessage_.getReaderBuffer().getData()[currentTransactionOffset_];
							
					// Advance to the next byte (wrap around if overflow)
					currentTransactionOffset_ = (currentTransactionOffset_ + 1) % outgoingMessage_.getReaderBuffer().getSize();
				}
						
				// Put the byte into the data register to be sent
				registers.I2CS.DATA.reg = data;
			}
			else
			{
				// Receive the next byte from the master
				incomingMessage_.getWriterBuffer().getData()[currentTransactionOffset_] = registers.I2CS.DATA.reg;
						
				// Advance to the next byte (wrap around if overflow)
				currentTransactionOffset_ = (currentTransactionOffset_ + 1) % incomingMessage_.getWriterBuffer().getSize();
			}
		}
	}
	

};


#endif /* I2CSLAVECONTROLLER_H_ */