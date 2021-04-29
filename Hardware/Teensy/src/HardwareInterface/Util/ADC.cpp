
#include "ADC.hxx"
#include "imxrt.h"

#include "Arduino.h"

void AmpedUp::Adc::initialize()
{
    // Calibrate ADC
	ADC1_GC = ADC_GC_CAL;
	while (ADC1_GC & ADC_GC_CAL);

    // Enable overwriting previous data
    ADC1_CFG |= ADC_CFG_OVWREN;

    // Set up conversion complete interrupt
    attachInterruptVector(IRQ_ADC1, AmpedUp::Adc::sampleCompleteInterruptHandler);
    NVIC_SET_PRIORITY(IRQ_ADC1, 15);
    NVIC_ENABLE_IRQ(IRQ_ADC1);
}

void AmpedUp::Adc::addChannel(uint8_t channel)
{
    activeChannels_[activeChannelCount_] = channel;
    activeChannelCount_++;
}

void AmpedUp::Adc::sampleAsync()
{
    if (activeChannelCount_ > 0)
    {
        currentChannelIndex_ = 0;
        ADC1_HC0 = activeChannels_[0] | ADC_HC_AIEN;
    }
}

float AmpedUp::Adc::getSample(uint8_t channel)
{
    return analogValues_[channel];
}

void AmpedUp::Adc::sampleCompleteInterruptHandler()
{

    static constexpr float MAX_VAL = 1023.0f;

    uint8_t currentChannel = activeChannels_[currentChannelIndex_];
    uint16_t sampleValueInt = ADC1_R0;
    analogValues_[currentChannel] = static_cast<float>(sampleValueInt) / MAX_VAL;

    currentChannelIndex_++;
    if (currentChannelIndex_ < activeChannelCount_)
    {
        ADC1_HC0 = activeChannels_[currentChannelIndex_] | ADC_HC_AIEN;
    }
}