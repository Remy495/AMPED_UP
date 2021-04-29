

#ifndef _WINDOW_SAMPLER_
#define _WINDOW_SAMPLER_

#include "AudioUtil.hpp"
#include "AudioWindow.hpp"

namespace AmpedUp
{
    class WindowSampler
    {
    public:
        static void initialize(uint8_t adcChannel, float sampleRateHz);

        static void freezeWindow();

        static void unfreezeWindow();

        static AudioUtil::sample_t* getCurrentWindow();

        static float* getPedalWindow();

        static void freezePedal();

        static void unfreezePedal();

    private:

        static inline AudioWindow audioWindow_{};
        static inline SlidingWindow<float, 1200> pedalWindow_{};
        static inline uint8_t adcChannel_{};

        static inline bool isPedalFrozen_{};

        static void timerInterrupt();

    };
}

#endif