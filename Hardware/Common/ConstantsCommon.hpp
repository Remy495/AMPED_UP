

#ifndef _CONSTANTS_COMMON_
#define _CONSTANTS_COMMON_

#include <cstdint>

namespace AmpedUp
{

    //////////////////////////////////////////////////////////////////////////////////
	///
	/// @brief Contains constants that are shared between the Teensy, the ESP32, and the Cortex M0s
	///
    namespace Constants
    {
        constexpr uint32_t DAUGHTER_BOARD_COUNT = 8;
        constexpr uint8_t DAUGHTER_BOARD_BASE_ADDRESS = 0x70;

        constexpr uint32_t REMOTE_MESSAGE_MTU = 1024;
        constexpr uint32_t REMOTE_MESSAGE_MAX_SIZE = 0xFFFF;
    }

}

#endif