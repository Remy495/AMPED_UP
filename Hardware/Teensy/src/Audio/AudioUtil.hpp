

#ifndef _AUDIO_UTIL_
#define _AUDIO_UTIL_

#include <cstdint>

#include "SlidingWindow.hxx"

namespace AmpedUp
{
    class AudioUtil
    {
    public:

        static constexpr double SAMPLE_RATE_HZ = 20000.0;
        static constexpr uint32_t WINDOW_SIZE = 3500;
        static constexpr float MINIMUM_FREQUENCY_HZ = 70.0;
        static constexpr float MAXIMUM_FREQUENCY_HZ = 1500.0;

        using sample_t = float;

        struct AnalysisResults
        {
            float volume;
            float pitch;
        };

        static AnalysisResults analyze(sample_t* sampleData);

        static void init();

        static void printAudioData(sample_t* sampleData, uint32_t length);
        
    private:
    };
}


#endif