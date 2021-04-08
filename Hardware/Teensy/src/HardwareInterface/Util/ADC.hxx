

#ifndef _ADC_
#define _ADC_

#include <cstdint>

namespace AmpedUp
{
    class Adc
    {
    public:
        static void begin(uint8_t channel);

        static float getCurrentValue();
    };
}

#endif