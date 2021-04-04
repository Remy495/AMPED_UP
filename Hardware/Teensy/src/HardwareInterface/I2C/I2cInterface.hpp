

#ifndef _I2C_INTERFACE_
#define _I2C_INTERFACE_

#include "I2cConfig.hpp"

namespace AmpedUp
{

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Enumeration representing the different speeds supported by I2C interface
///
enum class I2cSpeed
{
    STANDARD_100K,
    FAST_400K,
    FAST_PLUS_1M
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Provides a simplified interface for performing basic actions using the low power I2C peripherals in master mode
///
template<I2cHandle HANDLE>
class I2cInterface
{
public:

    using InterruptHandler = void(*)();


    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Enable the I2C peripheral in master mode
	///
    static void enable(I2cSpeed speed)
    {
        // Enable 24 MHz clock
        CCM_CSCDR2 = (CCM_CSCDR2 & ~CCM_CSCDR2_LPI2C_CLK_PODF(63)) | CCM_CSCDR2_LPI2C_CLK_SEL;
        *Hardware::clockConfigRegister_ |= Hardware::clockConfigValue_;

        // Set up pins
        Hardware::pins_.enableAllPins();

        // Set up send / receive queue water marks to 0.
        // This means we get an interrupt when the transmit buffer is empty and when the receive buffer is not empty
        getRegisters().MFCR = LPI2C_MFCR_RXWATER(0) | LPI2C_MFCR_TXWATER(0);

        // Subscribe to interrupts for RX watermark, stop condition, and errors
        // TX watermark interrupt causes problems because the transmit buffer is obviously empty during a receive
        // transaction, so it should only be enabled when a transmit transaction is in progress.
        getRegisters().MIER = LPI2C_MIER_RDIE | LPI2C_MIER_SDIE | LPI2C_MIER_NDIE | LPI2C_MIER_ALIE | LPI2C_MIER_FEIE | LPI2C_MIER_PLTIE;
        
        // Set the speed of the bus clock
        setSpeed(speed);

        // Enable the I2C peripheral
        getRegisters().MCR |= LPI2C_MCR_MEN;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Disable the I2C peripheral
	///
    static void disable()
    {
        // Disable the I2C peripheral
        getRegisters().MCR &= !LPI2C_MCR_MEN;

        // Clear incoming and outgoing data
        getRegisters().MCR |= LPI2C_MCR_RTF | LPI2C_MCR_RRF;

    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Enable interrupts for the I2C peripheral
	///
    static void enableInterrupts()
    {
        NVIC_ENABLE_IRQ(Hardware::interruptId_);
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Disable interrupts for the I2C peripheral
	///
    static void disableInterrupts()
    {
        NVIC_DISABLE_IRQ(Hardware::interruptId_);
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Specify the interrupt handler to call when interrupts are triggered for this I2C peripheral
	///
    static void setInterruptHandler(InterruptHandler handler)
    {
        attachInterruptVector(Hardware::interruptId_, handler);
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the control registers for this I2C peripheral
	///
    static IMXRT_LPI2C_t& getRegisters()
    {
        return *Hardware::registers_;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Set the speed this I2C peripheral operates at
	///
    static void setSpeed(I2cSpeed speed)
    {
        IMXRT_LPI2C_t& registers = getRegisters();

        switch (speed)
        {
        case I2cSpeed::FAST_PLUS_1M:
            registers.MCCR0 = LPI2C_MCCR0_CLKHI(9) | LPI2C_MCCR0_CLKLO(10) | LPI2C_MCCR0_DATAVD(4) | LPI2C_MCCR0_SETHOLD(7);
            registers.MCFGR1 = LPI2C_MCFGR1_PRESCALE(0);
            registers.MCFGR2 = LPI2C_MCFGR2_FILTSDA(1) | LPI2C_MCFGR2_FILTSCL(1) | LPI2C_MCFGR2_BUSIDLE(2 * (10 + 7 + 2));
            registers.MCFGR3 = LPI2C_MCFGR3_PINLOW(15000 * 24 / 256 + 1);
            break;
        case I2cSpeed::FAST_400K:
            registers.MCCR0 = LPI2C_MCCR0_CLKHI(26) | LPI2C_MCCR0_CLKLO(28) | LPI2C_MCCR0_DATAVD(12) | LPI2C_MCCR0_SETHOLD(18);
            registers.MCFGR1 = LPI2C_MCFGR1_PRESCALE(0);
            registers.MCFGR2 = LPI2C_MCFGR2_FILTSDA(2) | LPI2C_MCFGR2_FILTSCL(2) | LPI2C_MCFGR2_BUSIDLE(2 * (28 + 18 + 2));
            registers.MCFGR3 = LPI2C_MCFGR3_PINLOW(15000 * 24 / 256 + 1);
            break;
        default:
            registers.MCCR0 = LPI2C_MCCR0_CLKHI(55) | LPI2C_MCCR0_CLKLO(59) | LPI2C_MCCR0_DATAVD(25) | LPI2C_MCCR0_SETHOLD(40);
            registers.MCFGR1 = LPI2C_MCFGR1_PRESCALE(1);
            registers.MCFGR2 = LPI2C_MCFGR2_FILTSDA(5) | LPI2C_MCFGR2_FILTSCL(5) | LPI2C_MCFGR2_BUSIDLE(2 * (59 + 40 + 2));
            registers.MCFGR3 = LPI2C_MCFGR3_PINLOW(15000 * 12 / 256 + 1);
            break;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Send a start condition to a slave
    ///
    /// @param[in] address The address of the slave to send a start condition to. Must be between 0x0 and 0x7F
    /// @param[in] isTransmit Whether to mark the start condition as write (true) or read (false)
	///
    static void sendStart(uint8_t address, bool isTransmit)
    {

        getRegisters().MCR |= LPI2C_MCR_MEN;

        if (isTransmit)
        {
            // Enable transmit low watermark interrupt
            getRegisters().MIER |= LPI2C_MIER_TDIE;
            // Send transmit start condition to address
            getRegisters().MTDR = LPI2C_MTDR_CMD_START | (address << START_CONDITION_ADDRESS_OFFSET) | START_CONDITION_TRANSMIT;
        }
        else
        {
            // Disable transmit low watermark interrupt, as we are not transmitting and don't need to worry about it
            getRegisters().MIER &= ~LPI2C_MIER_TDIE;
            // Send receive start condition to address
            getRegisters().MTDR = LPI2C_MTDR_CMD_START | (address << START_CONDITION_ADDRESS_OFFSET) | START_CONDITION_RECEIVE;
        }
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Send a byte of data over the I2C bus
	///
    static void sendByte(uint8_t byte)
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_TRANSMIT | byte;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Recieve one or more bytes of data over the I2C bus
    ///
    /// @param[in] byteCount The number of bytes to receive; must be greater than zero. Receiving 0 bytes is undefined behavior.
	///
    static void receiveBytes(uint8_t byteCount)
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_RECEIVE | (byteCount - 1);
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Send a stop condition
	///
    static void sendStop()
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_STOP;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Take the next received byte from the receive FIFO
	///
    static uint8_t takeReceivedByte()
    {
        return getRegisters().MRDR & 0xFF;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether there is data available in the receive FIFO
	///
    static bool hasReceivedData()
    {
        return incomingByteCount() > 0;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Check whether there is space available in the transmit FIFO
	///
    static bool canTransmitData()
    {
        return outgoingByteCount() < FIFO_SIZE;
    }

private:
    using Hardware = I2cConfig<HANDLE>;

    static constexpr uint32_t START_CONDITION_TRANSMIT = 0;
    static constexpr uint32_t START_CONDITION_RECEIVE = 1;
    static constexpr uint32_t START_CONDITION_ADDRESS_OFFSET = 1;

    static constexpr uint32_t FIFO_SIZE = 4;

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the number of actions waiting in the transmit FIFO
	///
    static uint8_t outgoingByteCount()
    {
        // TXCOUNT field from master fifo status register
        return getRegisters().MFSR & 0x7;
    }

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the number of bytes waiting in the receive FIFO
	///
    static uint8_t incomingByteCount()
    {
        // RXCOUNT field from master fifo status register
        return (getRegisters().MFSR >> 16) & 0x07;
    }
};


}

#endif