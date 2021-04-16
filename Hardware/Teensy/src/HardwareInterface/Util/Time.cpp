
#include "imxrt.h"

#include "Time.hpp"

uint32_t AmpedUp::Time::overflowCount_ = 0;

// Size (in bits) of the GPT counter register
static constexpr uint64_t COUNTER_SIZE_BITS = 32;

// Frequency of GPT clock (24MHz)
static constexpr uint32_t TIMER_HZ = 24000000;


AmpedUp::Time::Time() = default;

AmpedUp::Time::Time(uint64_t ticks) : ticks_(ticks)
{}

AmpedUp::Time::~Time() = default;

double AmpedUp::Time::getSeconds()
{
    return static_cast<double>(ticks_) / static_cast<double>(TIMER_HZ);
}

double AmpedUp::Time::getMiliseconds() const
{
    return 1000.0 * static_cast<double>(ticks_) / static_cast<double>(TIMER_HZ);
}

double AmpedUp::Time::getMicroseconds() const
{
    return 1000000.0 * static_cast<double>(ticks_) / static_cast<double>(TIMER_HZ);
}

uint64_t AmpedUp::Time::getTicks() const
{
    return ticks_;
}

void AmpedUp::Time::initialize()
{
    // Set GPT to use high frequency (24MHz) clock
    GPT1_CR |= GPT_CR_CLKSRC(2);
    // Set GPT to reset when enabled
    GPT1_CR |= GPT_CR_ENMOD;
    // Set GPT to keep counting indefinitely / overflow
    GPT1_CR |= GPT_CR_FRR;
    // Set GPT to trigger interupt on overflow
    GPT1_IR |= GPT_IR_ROVIE;

    // Start GPT
    GPT1_CR |= GPT_CR_EN;

    // Enable interrupts for GPT
    attachInterruptVector(IRQ_GPT1, AmpedUp::Time::overflowInterupt);
	NVIC_SET_PRIORITY(IRQ_GPT1, 0);
	NVIC_ENABLE_IRQ(IRQ_GPT1);
}

AmpedUp::Time AmpedUp::Time::now()
{
    return Time(getCurrentTimeTicks());
}

void AmpedUp::Time::delay(const Time& time)
{
    uint64_t endTimeTicks = getCurrentTimeTicks() + time.getTicks();
    while(getCurrentTimeTicks() < endTimeTicks);
}

void AmpedUp::Time::waitUntil(const Time& time)
{
    uint64_t endTimeTicks = time.getTicks();
    while(getCurrentTimeTicks() < endTimeTicks);
}

uint64_t AmpedUp::Time::getCurrentTimeTicks()
{
    uint64_t ticks{0};

    // GPT counter is only 32 bits, which at high resolution overflows every couple minutes. In order to ensure a
    // consistant, monotonic sense of time, we can build a 64 bit number with the GPT counter as the lower 32 bits and
    // the number of overflows as the upper 32 bits

    ticks |= GPT1_CNT;
    ticks |= static_cast<uint64_t>(overflowCount_) << COUNTER_SIZE_BITS;

    return ticks;
}

void AmpedUp::Time::overflowInterupt()
{
    // Incrememt overflow counter every time GPT counter overflows
    if (GPT1_SR & GPT_SR_ROV)
    {
        overflowCount_++;
    }

    // Mark overflow interrupt as handled
    GPT1_SR = GPT_SR_ROV;
}

uint64_t operator""_m(long double minutes)
{
    return minutes * TIMER_HZ / 60.0;
}

uint64_t operator""_s(long double seconds)
{
    return seconds * TIMER_HZ;
}

uint64_t operator""_ms(long double milis)
{
    return milis * TIMER_HZ / 1000.0;
}

uint64_t operator""_us(long double micros)
{
    return micros * TIMER_HZ / 1000000.0;
}