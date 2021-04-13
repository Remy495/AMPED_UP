

#ifndef _SPI_MANAGER_
#define _SPI_MANAGER_

#include "driver/gpio.h"


namespace AmpedUp
{
    class SpiManager
    {
    public:

        static bool isInitialized();

        static bool run(gpio_num_t mosiPin, gpio_num_t misoPin, gpio_num_t sckPin, gpio_num_t ssPin, gpio_num_t handshakePin);

        static void setHandshakePin(uint32_t level);

        static void notifyNewConnection();

        static void notifyNewDisconnection();

    private:

        static inline bool newConnectionWasMade_{};

        static inline bool connectionWasJustLost_{};

        static inline bool isInitialized_{false};
        static inline gpio_num_t handshakePin_{};
    };
}

#endif