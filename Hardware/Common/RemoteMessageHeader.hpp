

#ifndef _REMOTE_MESSAGE_HEADER_
#define _REMOTE_MESSAGE_HEADER_

#include <cstdint>
#include <ostream>

namespace AmpedUp
{

    using RemoteMessageSize_t = uint16_t;

    enum class RemoteMessageFlag : uint8_t
    {
        REMOTE_CONNECTED,
        REMOTE_DISCONNECTED,
        LAST_FRAGMENT,
        FIRST_FRAGMENT,
        READY_TO_RECIEVE
    };

    class __attribute__((__packed__)) RemoteMessageHeader
    {
    public:

        class FlagGroup
        {
        public:

            constexpr FlagGroup() = default;

            constexpr bool contains(RemoteMessageFlag f) const
            {
                return flagMasks_ & getBitmask(f);
            }

            constexpr void addFlag(RemoteMessageFlag f)
            {
                flagMasks_ |= getBitmask(f);
            }

            constexpr void clearFlags()
            {
                flagMasks_ = 0;
            }

            uint8_t flagMasks_{};

        private:

            static constexpr uint8_t getBitmask(RemoteMessageFlag flag)
            {
                return 1U << static_cast<uint8_t>(flag);
            }

        };

        constexpr RemoteMessageHeader() = default;

        constexpr RemoteMessageHeader(FlagGroup messageFlags) : flags_(messageFlags)
        {}

        constexpr RemoteMessageHeader(FlagGroup messageFlags, RemoteMessageSize_t payloadSize) : totalPayloadSize_(payloadSize), fragmentPayloadSize_(payloadSize), flags_(messageFlags)
        {}

        constexpr RemoteMessageHeader(FlagGroup messageFlags, RemoteMessageSize_t totalPayloadSize, RemoteMessageSize_t fragmentPayloadSize) : totalPayloadSize_(totalPayloadSize), fragmentPayloadSize_(fragmentPayloadSize), flags_(messageFlags)
        {}

        constexpr FlagGroup getFlags() const
        {
            return flags_;
        }

        constexpr bool hasFlag(RemoteMessageFlag f) const
        {
            return flags_.contains(f);
        }

        constexpr void setFlag(RemoteMessageFlag f)
        {
            flags_.addFlag(f);
        }

        constexpr void clearFlags()
        {
            flags_.clearFlags();
        }

        constexpr RemoteMessageSize_t getTotalPayloadSize() const
        {
            return totalPayloadSize_;
        }

        constexpr void setTotalPayloadSize(RemoteMessageSize_t size)
        {
            totalPayloadSize_ = size;
        }

        constexpr RemoteMessageSize_t getFragmentPayloadSize() const
        {
            return fragmentPayloadSize_;
        }

        constexpr void setFragmentPayloadSize(RemoteMessageSize_t size)
        {
            fragmentPayloadSize_ = size;
        }

        constexpr bool hasPayload() const
        {
            return fragmentPayloadSize_ > 0;
        }
        
        constexpr bool isFragmented() const
        {
            return !(hasFlag(RemoteMessageFlag::FIRST_FRAGMENT) && hasFlag(RemoteMessageFlag::LAST_FRAGMENT));
        }

        constexpr bool isValid() const
        {
            return heartbeat_ == HEARTBEAT_VALUE;
        }

        constexpr void invalidate()
        {
            heartbeat_ = 0;
        }

    private:

        static constexpr uint16_t HEARTBEAT_VALUE = 0xD36C;

        RemoteMessageSize_t totalPayloadSize_{0};
        RemoteMessageSize_t fragmentPayloadSize_{0};
        FlagGroup flags_{};
        uint16_t heartbeat_{HEARTBEAT_VALUE};
    };

}

inline std::ostream& operator<<(std::ostream& os, const AmpedUp::RemoteMessageHeader& header)
{
    os << "(" << header.getFragmentPayloadSize() << " bytes / " << header.getTotalPayloadSize() << " bytes)";

    if (header.hasFlag(AmpedUp::RemoteMessageFlag::REMOTE_CONNECTED))
    {
        os << " CONNECTED";
    }

    if (header.hasFlag(AmpedUp::RemoteMessageFlag::REMOTE_DISCONNECTED))
    {
        os << " DISCONNECTED";
    }

    if (header.hasFlag(AmpedUp::RemoteMessageFlag::FIRST_FRAGMENT))
    {
        os << " FIRST";
    }

    if (header.hasFlag(AmpedUp::RemoteMessageFlag::LAST_FRAGMENT))
    {
        os << " LAST";
    }

    if (header.hasFlag(AmpedUp::RemoteMessageFlag::READY_TO_RECIEVE))
    {
        os << " READY";
    }

    return os;
}

#endif