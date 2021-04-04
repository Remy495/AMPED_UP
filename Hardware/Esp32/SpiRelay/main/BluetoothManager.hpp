

#ifndef _BLUETOOTH_MANAGER_
#define _BLUETOOTH_MANAGER_

#include "RemoteMessage.hpp"

namespace AmpedUp
{

    class BluetoothManager
    {
    public:

        static RemoteMessage& beginOutgoingMessage();

        static RemoteMessage& getOutgoingMessage();

        static void finishOutgoingMessage();

        static void cancelOutgoingMessage();

        static void messageProcessingFinished();

        static bool isInitialized();

        static bool run();
    private:
        static inline bool isInitialized_{false};

        static inline RemoteMessageSize_t currentMessageTotalSize_{};
        static inline RemoteMessageSize_t currentMessageRemainingSize_{}; 

        static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size);

    };

}

#endif