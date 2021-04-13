

#ifndef _TIME_
#define _TIME_

#include <cstdint>

namespace AmpedUp
{
    class Time
    {

    public:

        ///////////////////////////////////////////////////////
        /// @brief Connect this node input to a node output value
        ///
        static void initialize();

        static double getSeconds();

        static uint32_t overflowCount_;

    private:

        static uint64_t getTicks();

        static void overflowInterupt();
    };
}

#endif