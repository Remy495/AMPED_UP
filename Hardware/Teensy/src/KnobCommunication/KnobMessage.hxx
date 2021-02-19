
#ifndef _KNOB_MESSAGE_
#define _KNOB_MESSAGE_

#include <cstdint>

namespace AmpedUp
{

    enum class KnobMessageType : int8_t;
    {
        ERROR = -1,
        HEARTBEAT = 0,
        HEARTBEAT_REPLY,
        SET_KNOB_POSITION,
        CURRENT_KNOB_POSITION,
        DO_AUTO_CALIBRATION
    }


    // pack structure, put checksum before
    class KnobMessage
    {
    public:
        KnobMessage() = default;;

        KnobMessage(KnobMessageType type, float value = 0.0f) : value_(value), messageTypeRaw_(static_cast<int8_t>(type))
        {}

        ~KnobMessage() = default;

        KnobMessageType getMessageType() const
        {
            return static_cast<KnobMessageType>(messageTypeRaw_);
        }

        void setMessageType(KnobMessageType messageType)
        {
            messageTypeRaw_ = static_cast<int8_t>(messageType);
        }

        float getValue() const
        {
            return value_;
        }

        void setValue(float value)
        {
            value_ = value;
        }

    private:
        float value_;
        int8_t messageTypeRaw_;
    }

}


#endif