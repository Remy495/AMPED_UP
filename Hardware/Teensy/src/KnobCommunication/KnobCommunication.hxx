

#ifndef _KNOB_COMMUNICATION_
#define _KNOB_COMMUNICATION_

#include "imxrt.h"

#include "KnobMessage.hxx"
#include "StaticQueue.hxx"

namespace AmpedUp
{

    class KnobCommunicator
    {
    public:
        KnobCommunicator(const IMXRT_LPI2C_t& interface);

        ~KnobCommunicator();


    private:

        static constexpr uint32_t RX_HIGH_WATER_MARK = 16;
        static constexpr uint32_t TX_HIGH_WATER_MARK = 16;

        StaticQueue<KnobMessage, TX_HIGH_WATER_MARK> outgoingMessages_;

        

    };

}

#endif