

#ifndef _I2C_INTERFACE_
#define _I2C_INTERFACE_

#include "I2cConfig.hpp"

namespace AmpedUp
{

enum class I2cSpeed
{
    STANDARD_100K,
    FAST_400K,
    FAST_PLUS_1M
};

template<I2cHandle HANDLE>
class I2cInterface
{
public:

    using InterruptHandler = void(*)();

    static void enable()
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
    }

    static void disable()
    {

    }

    static void enableInterrupts()
    {
        NVIC_ENABLE_IRQ(Hardware::interruptId_);
    }

    static void disableInterrupts()
    {
        NVIC_DISABLE_IRQ(Hardware::interruptId_);
    }

    static void setInterruptHandler(InterruptHandler handler)
    {
        attachInterruptVector(Hardware::interruptId_, handler);
    }

    static IMXRT_LPI2C_t& getRegisters()
    {
        return *Hardware::registers_;
    }

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

    static void sendByte(uint8_t byte)
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_TRANSMIT | byte;
    }

    static void receiveBytes(uint8_t byteCount)
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_RECEIVE | (byteCount - 1);
    }

    static void sendStop()
    {
        getRegisters().MTDR = LPI2C_MTDR_CMD_STOP;
    }

    static uint8_t takeReceivedByte()
    {
        return getRegisters().MRDR & 0xFF;
    }

    static bool hasReceivedData()
    {
        return incomingByteCount() > 0;
    }

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

    static uint8_t outgoingByteCount()
    {
        // TXCOUNT field from master fifo status register
        return getRegisters().MFSR & 0x7;
    }

    static uint8_t incomingByteCount()
    {
        // RXCOUNT field from master fifo status register
        return (getRegisters().MFSR >> 16) & 0x07;
    }
};


}

#endif