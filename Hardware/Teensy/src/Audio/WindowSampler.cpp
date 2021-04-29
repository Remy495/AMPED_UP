

#include "WindowSampler.hpp"
#include "imxrt.h"
#include "Time.hpp"
#include "ADC.hxx"


void AmpedUp::WindowSampler::initialize(uint8_t adcChannel, float sampleRateHz)
{
    // Initialize the ADC
    adcChannel_ = adcChannel;
    Adc::initialize();
    Adc::addChannel(adcChannel);
    Adc::addChannel(2);
    
    // Initialize periodic timer to sample input regularly
    AmpedUp::Time sampleInterval = AmpedUp::Time::seconds(1 / sampleRateHz);
    PIT_LDVAL0 = sampleInterval.getTicks();

    PIT_TCTRL0 |= PIT_TCTRL_TEN | PIT_TCTRL_TIE;

    attachInterruptVector(IRQ_PIT, AmpedUp::WindowSampler::timerInterrupt);
    NVIC_SET_PRIORITY(IRQ_PIT, 20);
    NVIC_ENABLE_IRQ(IRQ_PIT);
}

void AmpedUp::WindowSampler::freezeWindow()
{
    NVIC_DISABLE_IRQ(IRQ_PIT);

    audioWindow_.freeze();

    NVIC_ENABLE_IRQ(IRQ_PIT);
}

void AmpedUp::WindowSampler::unfreezeWindow()
{
    NVIC_DISABLE_IRQ(IRQ_PIT);

    audioWindow_.unfreeze();

    NVIC_ENABLE_IRQ(IRQ_PIT);
}

AmpedUp::AudioUtil::sample_t* AmpedUp::WindowSampler::getCurrentWindow()
{
    return audioWindow_.getCurrentWindow();
}

void AmpedUp::WindowSampler::timerInterrupt()
{
    PIT_TFLG0 = 1;
    // Add the previous sample to the audio window
    float sampleValue = Adc::getSample(adcChannel_);
    audioWindow_.append(sampleValue);

    if (!isPedalFrozen_)
    {
        pedalWindow_.append(Adc::getSample(2));
    }

    // Begin next sample
    Adc::sampleAsync();
}

float* AmpedUp::WindowSampler::getPedalWindow()
{
    return pedalWindow_.getCurrentWindow();
}

void AmpedUp::WindowSampler::freezePedal()
{
    isPedalFrozen_ = true;
}  

void AmpedUp::WindowSampler::unfreezePedal()
{
    isPedalFrozen_ = false;
}