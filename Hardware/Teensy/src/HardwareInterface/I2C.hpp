
#ifndef _I2C_
#define _I2C_

#include <cstdint>

//TODO: Do not include C APIs in header files, as they end up getting included everywhere and clog up the global namespace
// In this case, the issue should probably be solved with a wrapper class around 
#include "imxrt.h"
#include "PinConfig.hpp"

namespace AmpedUp
{

    class I2CHardware
    {
    public:

        enum class InterfaceHandle
        {
            INTERFACE_1, // SDA = 18, SCL = 19
            // Interface 2 does not have its pads broken out on the Teensy
            INTERFACE_3, // SDA = 17, SCL = 16
            INTERFACE_4  // SDA = 25, SCL = 24
        };

        enum class SpeedMode
        {
            STANDERD, // 100 kHz
            FAST,     // 400 kHz
            FAST_PLUS // 1 MHz
        };

        void sendStart(uint8_t address);
        void sendStop();
        void sendByte();

        static I2CHardware& getInterface(InterfaceHandle handle);

    private:
        PinConfig sclPinConfig_;
        PinConfig sdaPinConfig_;

        IMXRT_LPI2C_t& registers_;

        I2CHardware(const PinConfig& sclPinConfig, const PinConfig& sdaPinConfig, )

    };

}

#endif