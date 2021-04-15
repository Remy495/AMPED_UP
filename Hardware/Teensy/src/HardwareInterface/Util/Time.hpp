

#ifndef _TIME_
#define _TIME_

#include <cstdint>

namespace AmpedUp
{
    class Time
    {

    public:

        Time();

        Time(uint64_t ticks);

        ~Time();

        double getSeconds();

        double getMiliseconds() const;

        double getMicroseconds() const;

        uint64_t getTicks() const;

        static void initialize();

        static Time now();

        static void delay(const Time& time);

        static void waitUntil(const Time& time);

    private:

        uint64_t ticks_{};

        static uint32_t overflowCount_;

        static uint64_t getCurrentTimeTicks();

        static void overflowInterupt();
    };
}

uint64_t operator""_m(long double minutes);

uint64_t operator""_s(long double seconds);

uint64_t operator""_ms(long double milis);

uint64_t operator""_us(long double micros);

#endif