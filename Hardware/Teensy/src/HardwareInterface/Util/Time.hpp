

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

        double getSeconds() const;

        double getMiliseconds() const;

        double getMicroseconds() const;

        uint64_t getTicks() const;

        bool operator<(const Time& other) const;

        bool operator>(const Time& other) const;

        bool operator<=(const Time& other) const;

        bool operator>=(const Time& other) const;

        Time& operator+=(const Time& other);

        Time& operator-=(const Time& other);

        static void initialize();

        static Time now();

        static Time seconds(float seconds);

        static Time miliseconds(float miliseconds);

        static Time microseconds(float microseconds);

        static void delay(const Time& time);

        static void waitUntil(const Time& time);

    private:

        uint64_t ticks_{};

        static uint32_t overflowCount_;

        static uint64_t getCurrentTimeTicks();

        static void overflowInterupt();
    };
}

AmpedUp::Time operator+(AmpedUp::Time lhs, const AmpedUp::Time& rhs);

AmpedUp::Time operator-(AmpedUp::Time lhs, const AmpedUp::Time& rhs);

uint64_t operator""_m(long double minutes);

uint64_t operator""_s(long double seconds);

uint64_t operator""_ms(long double milis);

uint64_t operator""_us(long double micros);

uint64_t operator""_m(unsigned long long minutes);

uint64_t operator""_s(unsigned long long seconds);

uint64_t operator""_ms(unsigned long long milis);

uint64_t operator""_us(unsigned long long micros);

#endif