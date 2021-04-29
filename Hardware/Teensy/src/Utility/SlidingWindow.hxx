

#ifndef _SLIDING_WINDOW_
#define _SLIDING_WINDOW_

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <ostream>

namespace AmpedUp
{
    template<typename T, uint32_t windowSize>
    class SlidingWindow
    {
    public:

        constexpr SlidingWindow() = default;

        ~SlidingWindow() = default;

        void append(const T& value)
        {
            buffer_[windowStartIndex_] = value;
            buffer_[windowStartIndex_ + windowSize] = value;
            windowStartIndex_ = (windowStartIndex_ + 1) % windowSize;
            if (windowOccupancy_ < windowSize)
            {
                windowOccupancy_++;
            }
        }

        T* getCurrentWindow()
        {
            return buffer_ + windowStartIndex_ + windowSize - windowOccupancy_;
        }

        const T* getCurrentWindow() const
        {
            return buffer_ + windowStartIndex_ + windowSize - windowOccupancy_;
        }

        uint32_t getCurrentWindowOccupancy() const
        {
            return windowOccupancy_;
        }

        uint32_t getWindowSize() const
        {
            return windowSize;
        }

        void append(const SlidingWindow<T, windowSize>& other)
        {
            const T* source = other.getCurrentWindow();
            uint32_t copySize = sizeof(T) * other.getCurrentWindowOccupancy();

            T* destination1 = buffer_ + windowStartIndex_;
            memcpy(destination1, source, copySize);

            if (windowStartIndex_ + other.getCurrentWindowOccupancy() < windowSize)
            {
                T* destination2 = destination1 + windowSize;
                memcpy(destination2, source, copySize);
            }

            windowStartIndex_ = (windowStartIndex_ + other.getCurrentWindowOccupancy()) % windowSize;
            windowOccupancy_ = std::min(windowOccupancy_ + other.getCurrentWindowOccupancy(), windowSize);
        }

        void clear()
        {
            windowOccupancy_ = 0;
            windowStartIndex_ = 0;
        }


    private:
        // Note: we only ever actually use the first windowSize * 2 - 1 spots in the buffer, but allocating an extra spot
        // is cheaper than a conditional in append() to make sure we don't overrun the buffer
        static constexpr uint32_t BUFFER_SIZE = windowSize * 2;

        T buffer_[BUFFER_SIZE];
        uint32_t windowStartIndex_{0};
        uint32_t windowOccupancy_{0};
    };
}

template<typename T, uint32_t windowSize>
std::ostream& operator<<(std::ostream& os, const AmpedUp::SlidingWindow<T, windowSize>& window)
{
    const T* windowData = window.getCurrentWindow();
    bool printSeparator = false;
    for (int i = 0; i < window.getCurrentWindowOccupancy(); i++)
    {
        if (printSeparator)
        {
            os << ", ";
        }

        os << windowData[i];
    }

    return os;
}

#endif