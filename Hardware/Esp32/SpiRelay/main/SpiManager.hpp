

#ifndef _SPI_MANAGER_
#define _SPI_MANAGER_

#include "driver/gpio.h"

#include "RemoteMessage.hpp"

namespace AmpedUp
{
    class SpiManager
    {
    public:
        
        static RemoteMessage& beginOutgoingMessage();

        static RemoteMessage& getOutgoingMessage();

        static void finishOutgoingMessage();

        static void cancelOutgoingMessage();

        static bool isInitialized();

        static bool run(gpio_num_t mosiPin, gpio_num_t misoPin, gpio_num_t sckPin, gpio_num_t ssPin, gpio_num_t handshakePin);

        static void setHandshakePin(uint32_t level);

    private:

        static inline bool isInitialized_{false};
        static inline gpio_num_t handshakePin_{};

        struct transmissionAttributes_t
        {
            uint32_t size_;
            uint8_t* dataPtr_;
        };

        static transmissionAttributes_t getTransmissionAttributes(RemoteMessage& spiMessage, bool sendHeader);

    };
}

#endif