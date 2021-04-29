

#ifndef _ADC_
#define _ADC_

#include <cstdint>

namespace AmpedUp
{
    class Adc
    {
    public:

        static void initialize();

        static void addChannel(uint8_t channel);

        static void sampleAsync();

        static float getSample(uint8_t channel);

    private:

        static inline uint8_t activeChannels_[16];
        static inline uint8_t activeChannelCount_{0};
        static inline volatile float analogValues_[16];

        static inline volatile uint8_t currentChannelIndex_{};

        static void sampleCompleteInterruptHandler();
    };
}

#endif