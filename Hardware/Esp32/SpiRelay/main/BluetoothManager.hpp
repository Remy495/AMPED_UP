

#ifndef _BLUETOOTH_MANAGER_
#define _BLUETOOTH_MANAGER_

#include "CircularQueue.hpp"
#include "BinaryUtil.hxx"

namespace AmpedUp
{

    class BluetoothManager
    {
    public:

        static bool readyToSend();

        static void sendNewMessage(const BinaryUtil::byte_t* data, RemoteMessageSize_t totalMessageSize, RemoteMessageSize_t fragmentMessageSize);

        static void sendContinuingMessage(const BinaryUtil::byte_t* data, RemoteMessageSize_t fragmentMessageSize);

        static RemoteMessageSize_t getIncomingMessageSize();

        static RemoteMessageSize_t getIncomingMessageAvailableSize();

        static void recieveIncomingMessage(BinaryUtil::byte_t* data, RemoteMessageSize_t fragmentMessageSize);

        static bool isConnected();

        static bool isInitialized();

        static bool run();
    private:

        static inline CircularQueue outgoingDataQueue_{};
        static inline CircularQueue incomingDataQueue_{};
        static inline bool isConnected_{};
        static inline uint16_t currentRfcommChannel_{};
        static inline uint8_t incomingCreditCount_{};
        static inline bool isInitialized_{false};

        static inline RemoteMessageSize_t currentMessageTotalSize_{};
        static inline RemoteMessageSize_t currentMessageRemainingSize_{}; 

        static void sendIfNeededExternal();

        static void sendIfNeededInternal(void* unused = nullptr);

        static void grantCreditsExternal();

        static void grantCreditsInternal(void* unused = nullptr);

        static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size);

    };

}

#endif