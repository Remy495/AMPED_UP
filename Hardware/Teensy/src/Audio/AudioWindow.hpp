

#ifndef _AUDIO_WINDOW_
#define _AUDIO_WINDOW_

#include "AudioUtil.hpp"
#include "SlidingWindow.hxx"

namespace AmpedUp
{
    class AudioWindow
    {
    public:

        AudioWindow() = default;

        ~AudioWindow() = default;

        void append(AudioUtil::sample_t sample)
        {
            if (isFrozen_)
            {
                backgroundWindow().append(sample);
            }
            else
            {
                mainWindow().append(sample);
            }
        }

        AudioUtil::sample_t* getCurrentWindow()
        {
            return mainWindow().getCurrentWindow();
        }

        void freeze()
        {
            backgroundWindow().clear();
            isFrozen_ = true;
        }

        void unfreeze()
        {
            if (backgroundWindow().getCurrentWindowOccupancy() < AudioUtil::WINDOW_SIZE)
            {
                mainWindow().append(backgroundWindow());
            }
            else
            {
                swap();
            }
            isFrozen_ = false;
        }

    private:

        using AudioSlidingWindow = SlidingWindow<AudioUtil::sample_t, AudioUtil::WINDOW_SIZE>;

        AudioSlidingWindow window1_{};
        AudioSlidingWindow window2_{};
        bool swap_{};

        bool isFrozen_{};

        AudioSlidingWindow& mainWindow()
        {
            return swap_ ? window2_ : window1_;
        }

        AudioSlidingWindow& backgroundWindow()
        {
            return swap_ ? window1_ : window2_;
        }

        void swap()
        {
            swap_ = !swap_;
        }
    };
}

#endif