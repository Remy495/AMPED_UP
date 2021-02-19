

#ifndef PINS_H_
#define PINS_H_

#include <cstdint>

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Uniquely identifies a hardware port (containing a set of pins)
/// @note The corresponding numeric value for each enum member is the same as the numeric value used by the hardware to identify the corresponding port
///
enum class PinPort : uint8_t
{
	PORT_A = 0,
	PORT_B = 1
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Uniquely identifies a hardware pin
///
struct Pin
{
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Default initializer needed to support aggregate initialization
	///
	constexpr Pin() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Create a pin identifier
	/// 
	/// @param[in] port The port this pin is in
	/// @param[in] pinNumber the number of the pin within that port
	///
	constexpr Pin(PinPort port, uint8_t pinNumber) : port_(port), pinNumber_(pinNumber)
	{}
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Destructor
	///
	~Pin() = default;
	
	//////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Get the hardware port number for this pin
	///
	constexpr uint8_t getPortNumber() const
	{
		return static_cast<uint8_t>(port_);
	}
		
	PinPort port_{};
	uint8_t pinNumber_{};
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Lists of constants representing each of the pins exposed by the hardware by a recognizable name
///
namespace Pins
{
	// This is a complete list of the IO pins that are available on the SAMD21E
	
	constexpr Pin PA00 = {PinPort::PORT_A, 0};
	constexpr Pin PA01 = {PinPort::PORT_A, 1};
	constexpr Pin PA02 = {PinPort::PORT_A, 2};
	constexpr Pin PA03 = {PinPort::PORT_A, 3};
	constexpr Pin PA04 = {PinPort::PORT_A, 4};
	constexpr Pin PA05 = {PinPort::PORT_A, 5};
	constexpr Pin PA06 = {PinPort::PORT_A, 6};
	constexpr Pin PA07 = {PinPort::PORT_A, 7};
	constexpr Pin PA08 = {PinPort::PORT_A, 8};
	constexpr Pin PA09 = {PinPort::PORT_A, 9};
	constexpr Pin PA10 = {PinPort::PORT_A, 10};
	constexpr Pin PA11 = {PinPort::PORT_A, 11};
	constexpr Pin PA14 = {PinPort::PORT_A, 14};
	constexpr Pin PA15 = {PinPort::PORT_A, 15};
	constexpr Pin PA16 = {PinPort::PORT_A, 16};
	constexpr Pin PA17 = {PinPort::PORT_A, 17};
	constexpr Pin PA18 = {PinPort::PORT_A, 18};
	constexpr Pin PA19 = {PinPort::PORT_A, 19};
	constexpr Pin PA22 = {PinPort::PORT_A, 22};
	constexpr Pin PA23 = {PinPort::PORT_A, 23};
	constexpr Pin PA24 = {PinPort::PORT_A, 24};
	constexpr Pin PA25 = {PinPort::PORT_A, 25};
	constexpr Pin PA27 = {PinPort::PORT_A, 27};
	constexpr Pin PA28 = {PinPort::PORT_A, 28};
	constexpr Pin PA30 = {PinPort::PORT_A, 30};
	constexpr Pin PA31 = {PinPort::PORT_A, 31};
}



#endif /* PINS_H_ */