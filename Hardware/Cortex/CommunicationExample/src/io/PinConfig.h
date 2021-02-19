

#ifndef __PINCONFIG_H__
#define __PINCONFIG_H__

#include <cstdint>
#include "Pins.h"
#include "sam.h"

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Enumeration of the different functions a pin can be used for.
/// @note For modes where the pin is controlled by a peripheral (i.e. all but INPUT and OUTPUT), the corresponding numeric value of the enum member is the appropriate value for the PMUX register
///
enum class PinMode : uint8_t
{
	EXTERNAL_INTERRUPT = PORT_PMUX_PMUXE_A_Val,
	DAC_OR_ADC = PORT_PMUX_PMUXE_B_Val,
	SERCOM = PORT_PMUX_PMUXE_C_Val,
	SERCOM_ALT = PORT_PMUX_PMUXE_D_Val,
	TIMER_COUNTER = PORT_PMUX_PMUXE_E_Val,
	TIMER_COUNTER_CONTROL = PORT_PMUX_PMUXE_F_Val,
	COM = PORT_PMUX_PMUXE_G_Val,
	GENERIC_CLOCK = PORT_PMUX_PMUXE_H_Val,
	INPUT, // Basic PORT IO
	OUTPUT // Basic PORT IO
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents the different possible drive strengths for an output pin
///
enum class OutputDriveStrength : uint8_t
{
	NORMAL,
	HIGH
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents the different possible states of the pull resistor for an input pin
///
enum class InputPullResistorMode : uint8_t
{
	NONE,
	PULL_UP,
	PULL_DOWN
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Represents a possible configuration of a particular hardware pin and allows that configuration to be easily applied
///
class PinConfiguration
{
public:

	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a pin configuration
	/// 
	/// @param[in] pin The pin to configure
	/// @param[in] pinMode The function of the pin (i.e. an input, an output, a peripheral function, etc)
	/// @param[in] driveStrength The drive strength to use if / when this pin is an output (defaults to normal)
	/// @param[in] pullRes The type of pull resistor to use if / when this pin is an input (defaults to none)
	///
	constexpr PinConfiguration(Pin pin, PinMode pinMode, OutputDriveStrength driveStrength = OutputDriveStrength::NORMAL, InputPullResistorMode pullRes = InputPullResistorMode::NONE)
	{
		pinMode_ = pinMode;
		pullRes_ = pullRes;
		pin_ = pin;
		
		// Enable pin configuration in the WRCONFIG value
		wrConfigVal_ |= PORT_WRCONFIG_WRPINCFG;
		
		// Set the pin number in the WRCONFIG value
		uint16_t pinBitmaskHalf = 1;
		if (pin_.pinNumber_ < BITS_IN_HALFWORD)
		{
			pinBitmaskHalf <<= pin_.pinNumber_;
		}
		else
		{
			wrConfigVal_ |= PORT_WRCONFIG_HWSEL;
			pinBitmaskHalf <<= (pin_.pinNumber_ - BITS_IN_HALFWORD);
		}
		wrConfigVal_ |= PORT_WRCONFIG_PINMASK(pinBitmaskHalf);
		
		// If this pin is controlled by a peripheral, rather than the PORT IO controller, set pmux appropriately
		if (isPeripheralControlled())
		{
			wrConfigVal_ |= PORT_WRCONFIG_WRPMUX | PORT_WRCONFIG_PMUXEN;
			wrConfigVal_ |= PORT_WRCONFIG_PMUX(static_cast<uint8_t>(pinMode));
		}
		
		// Set the drive strength in the WRCONFIG value
		if (driveStrength == OutputDriveStrength::HIGH)
		{
			wrConfigVal_ |= PORT_WRCONFIG_DRVSTR;
		}
		
		// Set the input buffer enable / disable in the WRCONFIG value
		if (pinMode == PinMode::INPUT)
		{
			wrConfigVal_ |= PORT_WRCONFIG_INEN;
		}
		
		// Enable / disable the pull resistor
		if (pullRes != InputPullResistorMode::NONE)
		{
			wrConfigVal_ |= PORT_WRCONFIG_PULLEN;
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the pin that this pin configuration is for
	///
	constexpr Pin getPin() const
	{
		return pin_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the port the pin this pin configuration targets belongs to
	///
	constexpr PinPort getPort() const
	{
		return static_cast<PinPort>(pin_.getPortNumber());
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the pin number of the pin this pin configuration targets
	///
	constexpr uint8_t getPinNumber() const
	{
		return pin_.pinNumber_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get a bitmask for the pin this pin configuration targets
	///
	constexpr uint32_t getPinBitmask() const
	{
		return 1U << pin_.pinNumber_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get whether or not the pin is configured to be controlled by a peripheral device (such as a sercom)
	/// @return True if the pin is controlled by a peripheral, false if the pin is a simple PORT IO pin
	///
	constexpr bool isPeripheralControlled() const
	{
		return pinMode_ != PinMode::INPUT && pinMode_ != PinMode::OUTPUT;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the mode the pin is configured to be in
	///
	constexpr PinMode getPinMode() const
	{
		return pinMode_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the drive strength for the pin
	///
	constexpr OutputDriveStrength getOutputDriveStrength() const
	{
		return (wrConfigVal_ & PORT_WRCONFIG_DRVSTR) ? OutputDriveStrength::HIGH : OutputDriveStrength::NORMAL;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the pull resistor type for the pin
	///
	constexpr InputPullResistorMode getInputPullResistorMode() const
	{
		return pullRes_;
	}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Apply the pin configuration
	/// @details Actually configure the targeted pin as specified by this pin configuration
	///
	inline void apply() const
	{
		// Set the data direction
		if (pinMode_ == PinMode::OUTPUT)
		{
			PORT->Group[pin_.getPortNumber()].DIRSET.reg = getPinBitmask();
		}
		else if (pinMode_ == PinMode::INPUT)
		{
			PORT->Group[pin_.getPortNumber()].DIRCLR.reg = getPinBitmask();
				
			// If this is an input, set the pull resistor direction as well
			if (pullRes_ == InputPullResistorMode::PULL_UP)
			{
				PORT->Group[pin_.getPortNumber()].OUTSET.reg = getPinBitmask();
			}
			else if (pullRes_ == InputPullResistorMode::PULL_DOWN)
			{
				PORT->Group[pin_.getPortNumber()].OUTCLR.reg = getPinBitmask();
			}
		}
			
		// Set the pin configuration
		PORT->Group[pin_.getPortNumber()].WRCONFIG.reg = wrConfigVal_;
	}
	

private:
	static constexpr uint8_t BITS_IN_WORD = 32;
	static constexpr uint8_t BITS_IN_HALFWORD = BITS_IN_WORD / 2;

	uint32_t wrConfigVal_{0};
	PinMode pinMode_{PinMode::INPUT};
	InputPullResistorMode pullRes_{InputPullResistorMode::NONE};
	Pin pin_{};
};


#endif //__PINCONFIG_H__
