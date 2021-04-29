

#ifndef _REMOTE_COMMUNICATION_
#define _REMOTE_COMMUNICATION_

#include "flatbuffers/flatbuffers.h"
#include "AmpedUpMessaging_generated.h"

#include "TypedBuffer.h"
#include "RemoteMessageAllocator.hpp"

namespace AmpedUp
{

    class SpiPayload;

    class RemoteCommunication
    {
    public:

        static void initialize(uint32_t highWaterMark);

        static bool isReadyToSend(bool isImportant);

        static flatbuffers::FlatBufferBuilder& beginOutgoingMessage();

        static void sendFinishedMessage();

        static bool hasRecievedMessage();

        static const AmpedUpMessaging::Message* getRecievedMessage();

        static void discardRecievedMessage();

        static bool remoteIsConnected();

    private:

        static bool verifyMessage(const SpiPayload& messagePayload);

        static inline bool remoteIsConnected_{};
        static inline bool outgoingMessageInProgress_{};
        static inline uint32_t highWaterMark_{};
        static inline RemoteMessageAllocator allocator_{};
        static inline TypedBuffer<flatbuffers::FlatBufferBuilder> currentMessageBuilderContainer_{};
    };
}

#endif