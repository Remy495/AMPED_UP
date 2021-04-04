
#include "ADC.hxx"
#include "imxrt.h"

static constexpr float MAX_VAL = 1023.0f;

void AmpedUp::Adc::begin(uint8_t channel)
{
	ADC1_GC = ADC_GC_CAL;
	while (ADC1_GC & ADC_GC_CAL);

    ADC1_GC = ADC_GC_ADCO;
    ADC1_HC0 = channel;
}

float AmpedUp::Adc::getCurrentValue()
{
    return ADC1_R0 / MAX_VAL;
}