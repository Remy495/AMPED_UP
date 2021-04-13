

#ifndef _REMOTE_MESSAGE_HEADER_
#define _REMOTE_MESSAGE_HEADER_

#include <cstdint>

namespace AmpedUp
{

    enum class RemoteMessageType : uint8_t
    {
        NOTHING_TO_REPORT,
        REMOTE_CONNECTED,
        REMOTE_DISCONNECTED,
        REMOTE_MESSAGE
    };

    using RemoteMessageSize_t = uint16_t;

    class __attribute__((__packed__)) RemoteMessageHeader
    {
    public:
        RemoteMessageHeader(RemoteMessageType messageType) : messageType_(messageType)
        {}

        RemoteMessageHeader(RemoteMessageType messageType, RemoteMessageSize_t payloadSize) : totalPayloadSize_(payloadSize), fragmentPayloadSize_(payloadSize), messageType_(messageType)
        {}


        RemoteMessageHeader(RemoteMessageType messageType, RemoteMessageSize_t totalPayloadSize, RemoteMessageSize_t fragmentPayloadSize) : totalPayloadSize_(totalPayloadSize), fragmentPayloadSize_(fragmentPayloadSize), messageType_(messageType)
        {}

        RemoteMessageType getMessageType() const
        {
            return messageType_;
        }

        RemoteMessageSize_t getTotalPayloadSize() const
        {
            return totalPayloadSize_;
        }

        RemoteMessageSize_t getFragmentPayloadSize() const
        {
            return fragmentPayloadSize_;
        }

        bool getIsFragment() const
        {
            return totalPayloadSize_ > fragmentPayloadSize_;
        }

        bool hasPayload() const
        {
            return fragmentPayloadSize_ > 0;
        }

    private:
        RemoteMessageSize_t totalPayloadSize_{0};
        RemoteMessageSize_t fragmentPayloadSize_{0};
        RemoteMessageType messageType_{};
    };

}

#endif