

#ifndef SERCOMCONFIG_H_
#define SERCOMCONFIG_H_

#include "sam.h"

#include "PinConfigGroup.h"

// Integer type used to index hardware sercom units
using SercomIndex_t = uint8_t;

// Number of sercom units present on an ATSAMD21E15
static constexpr SercomIndex_t SERCOM_COUNT = 4;
	
//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Enumeration of handles used to uniquely identify a hardware sercom unit
///
enum class SercomHandle : SercomIndex_t
{
	SERCOM_0,
	SERCOM_1,
	SERCOM_2,
	SERCOM_3
};

// Pin config group to use to keep track of the pins assigned to each sercom
constexpr uint8_t SERCOM_PIN_COUNT = 4;
using SercomPinGroup = PinConfigGroup<SERCOM_PIN_COUNT>;

// Function pointer type for interrupt handler subroutine
using SercomIsr_t = void(*)();

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Consolidates constants, configuration info, and additional state (besides what is maintained by the hardware)
///        about each sercom unit into one place and presents a uniform interface to access it
///
/// @note Why this is an enum template rather than a normal class in an array (initialized statically): In a word,
///       optomizations. Most of the fields on SercomConfig can be constexpr, but not all. This means that
///       the table of isntances for SercomConfig could not be constexpr, which would mean that members may not be
///       initialized at compile time, resulting in significant overhead to use the sercom unit
///
template<SercomHandle HANDLE>
struct SercomConfig;

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Specialization for sercom 0
///
template<>
struct SercomConfig<SercomHandle::SERCOM_0>
{

    static inline Sercom* registersPtr = SERCOM0;

    static inline SercomIsr_t interruptHandler = nullptr;

    static inline constexpr IRQn_Type INTERRUPT_ID = SERCOM0_IRQn;

    static inline constexpr uint32_t POWER_MANAGER_BITMASK = PM_APBCMASK_SERCOM0;

    static inline constexpr uint32_t CLOCK_CONTROL_ID = SERCOM0_GCLK_ID_CORE;

    static inline constexpr SercomPinGroup DEFAULT_PINS =  {PinConfiguration(Pins::PA08, PinMode::SERCOM),
                                                            PinConfiguration(Pins::PA09, PinMode::SERCOM), 
                                                            PinConfiguration(Pins::PA10, PinMode::SERCOM),
                                                            PinConfiguration(Pins::PA11, PinMode::SERCOM)};
	
	static inline constexpr SercomPinGroup ALT_PINS =  {PinConfiguration(Pins::PA04, PinMode::SERCOM_ALT),
                                                        PinConfiguration(Pins::PA05, PinMode::SERCOM_ALT),
                                                        PinConfiguration(Pins::PA06, PinMode::SERCOM_ALT),
                                                        PinConfiguration(Pins::PA07, PinMode::SERCOM_ALT)};
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Specialization for sercom 1
///
template<>
struct SercomConfig<SercomHandle::SERCOM_1>
{

    static inline Sercom* registersPtr = SERCOM1;

    static inline SercomIsr_t interruptHandler = nullptr;

    static inline constexpr IRQn_Type INTERRUPT_ID = SERCOM1_IRQn;

    static inline constexpr uint32_t POWER_MANAGER_BITMASK = PM_APBCMASK_SERCOM1;

    static inline constexpr uint32_t CLOCK_CONTROL_ID = SERCOM1_GCLK_ID_CORE;

	static inline constexpr SercomPinGroup DEFAULT_PINS =  {PinConfiguration(Pins::PA16, PinMode::SERCOM),
										                    PinConfiguration(Pins::PA17, PinMode::SERCOM), 
										                    PinConfiguration(Pins::PA18, PinMode::SERCOM),
										                    PinConfiguration(Pins::PA19, PinMode::SERCOM)};
	
	static inline constexpr SercomPinGroup ALT_PINS =  {PinConfiguration(Pins::PA00, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA01, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA30, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA31, PinMode::SERCOM_ALT)};
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Specialization for sercom 2
///
template<>
struct SercomConfig<SercomHandle::SERCOM_2>
{

    static inline Sercom* registersPtr = SERCOM2;

    static inline SercomIsr_t interruptHandler = nullptr;

    static inline constexpr IRQn_Type INTERRUPT_ID = SERCOM2_IRQn;

    static inline constexpr uint32_t POWER_MANAGER_BITMASK = PM_APBCMASK_SERCOM2;

    static inline constexpr uint32_t CLOCK_CONTROL_ID = SERCOM2_GCLK_ID_CORE;

	static inline constexpr SercomPinGroup DEFAULT_PINS =  {PinConfiguration(Pins::PA08, PinMode::SERCOM_ALT),
										                    PinConfiguration(Pins::PA09, PinMode::SERCOM_ALT),
										                    PinConfiguration(Pins::PA14, PinMode::SERCOM),
										                    PinConfiguration(Pins::PA15, PinMode::SERCOM)};
	
	static inline constexpr SercomPinGroup ALT_PINS =  {PinConfiguration(Pins::PA08, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA09, PinMode::SERCOM_ALT), 
											            PinConfiguration(Pins::PA10, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA11, PinMode::SERCOM_ALT)};
};

//////////////////////////////////////////////////////////////////////////////////
///
/// @brief Specialization for sercom 3
///
template<>
struct SercomConfig<SercomHandle::SERCOM_3>
{

    static inline Sercom* registersPtr = SERCOM3;

    static inline SercomIsr_t interruptHandler = nullptr;

    static inline constexpr IRQn_Type INTERRUPT_ID = SERCOM3_IRQn;

    static inline constexpr uint32_t POWER_MANAGER_BITMASK = PM_APBCMASK_SERCOM3;

    static inline constexpr uint32_t CLOCK_CONTROL_ID = SERCOM3_GCLK_ID_CORE;

	static inline constexpr SercomPinGroup DEFAULT_PINS =  {PinConfiguration(Pins::PA22, PinMode::SERCOM),
										                    PinConfiguration(Pins::PA23, PinMode::SERCOM), 
										                    PinConfiguration(Pins::PA24, PinMode::SERCOM),
										                    PinConfiguration(Pins::PA25, PinMode::SERCOM)};
	
	static inline constexpr SercomPinGroup ALT_PINS =  {PinConfiguration(Pins::PA16, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA17, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA18, PinMode::SERCOM_ALT),
											            PinConfiguration(Pins::PA19, PinMode::SERCOM_ALT)};
};

#endif /* SERCOMCONFIG_H_ */