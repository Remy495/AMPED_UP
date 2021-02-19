

#ifndef __SERCOMCONTROLLER_H__
#define __SERCOMCONTROLLER_H__

#include "SercomConfig.h"

template<SercomHandle HANDLE>
class SercomInterface
{
public:

    SercomInterface() = delete;

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Enable / initialize the sercom hardware
	/// @note Calling this function does not enable any of the sercom's pins, that must be done separately via the SercomPinGroups
	///
    static inline void enable()
    {
        // Enable via the power manager
        PM->APBCMASK.reg |= SercomConfig<HANDLE>::POWER_MANAGER_BITMASK;
        
        // Send CPU clock to this sercom
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SercomConfig<HANDLE>::CLOCK_CONTROL_ID) | 
                            GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(0);
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Disable the sercom hardware
	///
    static inline void disable()
    {
        // Disable via the power manager
        PM->APBCMASK.reg &= !SercomConfig<HANDLE>::POWER_MANAGER_BITMASK;
        
        // Unroute clock
        GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(SercomConfig<HANDLE>::CLOCK_CONTROL_ID);
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Specify a function to handle interrupts produced by this sercom unit
	///
	/// @param[in] interruptHandler the function to call when an interrupt occurs
	///
    static inline void setInterruptHandler(SercomIsr_t isr)
    {
        SercomConfig<HANDLE>::interruptHandler = isr;
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Enable interrupts for this sercom unit
	/// @note This function just tells the NVIC to allow this sercom to produce interrupts. Individual interrupts can be enabled / disabled via the sercom registers
	///
    static inline void enableInterrupts()
    {
        NVIC_EnableIRQ(SercomConfig<HANDLE>::INTERRUPT_ID);
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Disable interrupts for this sercom unit
	///
    static inline void disableInterrupts()
    {
        NVIC_DisableIRQ(SercomConfig<HANDLE>::INTERRUPT_ID);
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Access the control registers for this sercom unit
	///
    static inline Sercom& getRegisters()
    {
        return *SercomConfig<HANDLE>::registersPtr;
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the pin configurations for this sercom unit's pins
	///
    static inline const SercomPinGroup& getPins()
    {
        return SercomConfig<HANDLE>::DEFAULT_PINS;
    }

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the pin configurations for this sercom unit's alternate pins
	///
    static inline const SercomPinGroup& getAltPins()
    {
        return SercomConfig<HANDLE>::ALT_PINS;
    }

};

#endif //__SERCOMCONTROLLER_H__
