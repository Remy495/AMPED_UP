
#include <algorithm>

#include "AudioUtil.hpp"
#include "arm_math.h"

#include "Arduino.h"


extern bool printAudio;

static constexpr uint32_t FILLTER_COEFFICIENT_COUNT = 71;
static float FILTER_COEFFICIENTS[FILLTER_COEFFICIENT_COUNT] = {0.0027679,0.0017502,-0.00031582,-0.0028938,-0.005137,-0.0061479,-0.0053035,-0.0025355,0.001546,0.0057544,0.008653,0.009024,0.0063338,0.0010297,-0.0054601,-0.011069,-0.013705,-0.01197,-0.0057465,0.0035581,0.013238,0.019988,0.020906,0.014524,0.001549,-0.014964,-0.030111,-0.038264,-0.034542,-0.016259,0.016076,0.058508,0.10427,0.14517,0.17345,0.18354,0.17345,0.14517,0.10427,0.058508,0.016076,-0.016259,-0.034542,-0.038264,-0.030111,-0.014964,0.001549,0.014524,0.020906,0.019988,0.013238,0.0035581,-0.0057465,-0.01197,-0.013705,-0.011069,-0.0054601,0.0010297,0.0063338,0.009024,0.008653,0.0057544,0.001546,-0.0025355,-0.0053035,-0.0061479,-0.005137,-0.0028938,-0.00031582,0.0017502,0.0027679};
float filterStateBuffer[AmpedUp::AudioUtil::WINDOW_SIZE + FILLTER_COEFFICIENT_COUNT - 1];
static arm_fir_instance_f32 lowPass;

void AmpedUp::AudioUtil::init()
{
    arm_fir_init_f32(&lowPass, FILLTER_COEFFICIENT_COUNT, FILTER_COEFFICIENTS, filterStateBuffer, AmpedUp::AudioUtil::WINDOW_SIZE);
}

// float AmpedUp::AudioUtil::findVolume(sample_t* sampleData)
// {
//     for (uint32_t i = 0; i < WINDOW_SIZE; i++)
//     {
//         zeroRef[i] = 2 * (sampleData[i] - 0.5);
//     }
    
//     arm_fir_f32(&lowPass, zeroRef, sampleDataFiltered, WINDOW_SIZE);

//     if (printAudio)
//     {
//         Serial.println("Filtered:");
//         printAudioData(sampleDataFiltered, WINDOW_SIZE);
//     }

//     float result;
//     arm_rms_f32(sampleDataFiltered, WINDOW_SIZE, &result);
//     return result;
// }

static AmpedUp::AudioUtil::sample_t buffer1[AmpedUp::AudioUtil::WINDOW_SIZE];
static AmpedUp::AudioUtil::sample_t buffer2[AmpedUp::AudioUtil::WINDOW_SIZE];

static constexpr uint32_t correlationOutputSize = 2 * AmpedUp::AudioUtil::SAMPLE_RATE_HZ / AmpedUp::AudioUtil::MINIMUM_FREQUENCY_HZ;
static AmpedUp::AudioUtil::sample_t autocorrelResult[correlationOutputSize];
AmpedUp::AudioUtil::AnalysisResults AmpedUp::AudioUtil::analyze(sample_t* sampleData)
{
    // Level-shift and filter noise
    for (uint32_t i = 0; i < WINDOW_SIZE; i++)
    {
        buffer1[i] = 2 * (sampleData[i] - 0.5);
    }

    arm_fir_f32(&lowPass, buffer1, buffer2, WINDOW_SIZE);

    // Find volume
    float volume;
    arm_rms_f32(buffer2, WINDOW_SIZE, &volume);

    // Find pitch

    //Create reversed version of the signal
    for (uint32_t i = 0; i < WINDOW_SIZE; i++)
    {
        buffer1[i] = buffer2[WINDOW_SIZE - i - 1];
    }

    // Perform autocorrelation
    arm_conv_partial_f32(buffer1, WINDOW_SIZE, buffer2, WINDOW_SIZE, autocorrelResult - (WINDOW_SIZE - 1), WINDOW_SIZE - 1, correlationOutputSize);

    if (printAudio)
    {
        Serial.println("Autocorrelated:");
        printAudioData(autocorrelResult, correlationOutputSize);
    }

    // Find peak (i.e. where the pitch is)
    float peekDetectorThreshold = autocorrelResult[0] * 0.5;
    float peekDetectorShortCircuitThreshold = autocorrelResult[0] * 0.85;
    bool hasFoundPeek = false;
    float frequency = 0;
    float bestPeekLevel = 0;
    // note: skip first sample as it is not relevant to frequency detection
    for (uint32_t i = 1; i < correlationOutputSize; i++)
    {
        if (hasFoundPeek && autocorrelResult[i] <= autocorrelResult[i-1])
        {
            float currentFrequency = SAMPLE_RATE_HZ / static_cast<float>(i);
            if (currentFrequency > MAXIMUM_FREQUENCY_HZ)
            {
                break;
            }
            else if (autocorrelResult[i-1] > peekDetectorShortCircuitThreshold)
            {
                // If the peek we found is very good (very high correlation) return the frequency and stop looking
                frequency = currentFrequency;
                bestPeekLevel = autocorrelResult[i-1];
                break;
            }
            else if (autocorrelResult[i-1] > bestPeekLevel && (std::fabs(autocorrelResult[i-1] - bestPeekLevel) / ((autocorrelResult[i-1] + bestPeekLevel) / 2)) > 0.02)
            {
                // If the peek we found is not very good, keep track of it but also keep looking for better peaks
                // Also, to improve stability, only accept a mediocre peak if it is substandially better than the last one (by percent difference)
                frequency = currentFrequency;
                bestPeekLevel = autocorrelResult[i-1];
                hasFoundPeek = false;
            }
            else
            {
                // The peek we found is too high frequency or is unacceptable for whatever reason. Move on to the next one.
                hasFoundPeek = false;
            }
        }
        else if (!hasFoundPeek && autocorrelResult[i] > peekDetectorThreshold && autocorrelResult[i] > autocorrelResult[i-1])
        {
            hasFoundPeek = true;
        }
    }


    return {volume, frequency};
}

// static AmpedUp::AudioUtil::sample_t sampleDataNormal[AmpedUp::AudioUtil::WINDOW_SIZE];
// static AmpedUp::AudioUtil::sample_t sampleDataNormalReversed[AmpedUp::AudioUtil::WINDOW_SIZE];
// static constexpr uint32_t correlationOutputSize = 2 * AmpedUp::AudioUtil::SAMPLE_RATE_HZ / AmpedUp::AudioUtil::MINIMUM_FREQUENCY_HZ;

// static AmpedUp::AudioUtil::sample_t autocorrelResult[correlationOutputSize];

// float AmpedUp::AudioUtil::findPitch(sample_t* sampleData)
// {
//     float max, min;
//     uint32_t unused;

//     arm_max_f32(sampleData, WINDOW_SIZE, &max, &unused);
//     arm_min_f32(sampleData, WINDOW_SIZE, &min, &unused);

//     // Find autocorrelation of current window
//     // Using convolution on the reversed window because there is no partial correlation function
//     for (uint32_t i = 0; i < WINDOW_SIZE; i++)
//     {
//         sampleDataNormal[i] = (sampleData[i] - 0.5) / (max - min);
//         sampleDataNormalReversed[i] = (sampleData[WINDOW_SIZE - i - 1] - 0.5) / (max - min);
//     }

//     arm_conv_partial_f32(sampleData, WINDOW_SIZE, sampleDataNormalReversed, WINDOW_SIZE, autocorrelResult - (WINDOW_SIZE - 1), WINDOW_SIZE - 1, correlationOutputSize);

//     if (printAudio)
//     {
//         Serial.println("Autocorrelated:");
//         printAudioData(autocorrelResult, correlationOutputSize);
//     }

//     // Find peak
//     float peekDetectorThreshold = autocorrelResult[0] * 0.5;
//     float peekDetectorShortCircuitThreshold = autocorrelResult[0] * 0.85;
//     bool hasFoundPeek = false;
//     float frequency = 0;
//     float bestPeekLevel = 0;
//     // note: skip first sample as it is not relevant to frequency detection
//     for (uint32_t i = 1; i < correlationOutputSize; i++)
//     {
//         if (hasFoundPeek && autocorrelResult[i] <= autocorrelResult[i-1])
//         {
//             float currentFrequency = SAMPLE_RATE_HZ / static_cast<float>(i);
//             if (currentFrequency > MAXIMUM_FREQUENCY_HZ)
//             {
//                 break;
//             }
//             else if (autocorrelResult[i-1] > peekDetectorShortCircuitThreshold)
//             {
//                 // If the peek we found is very good (very high correlation) return the frequency and stop looking
//                 frequency = currentFrequency;
//                 bestPeekLevel = autocorrelResult[i-1];
//                 break;
//             }
//             else if (autocorrelResult[i-1] > bestPeekLevel && (std::fabs(autocorrelResult[i-1] - bestPeekLevel) / ((autocorrelResult[i-1] + bestPeekLevel) / 2)) > 0.02)
//             {
//                 // If the peek we found is not very good, keep track of it but also keep looking for better peaks
//                 // Also, to improve stability, only accept a mediocre peak if it is substandially better than the last one (by percent difference)
//                 frequency = currentFrequency;
//                 bestPeekLevel = autocorrelResult[i-1];
//                 hasFoundPeek = false;
//             }
//             else
//             {
//                 // The peek we found is too high frequency or is unacceptable for whatever reason. Move on to the next one.
//                 hasFoundPeek = false;
//             }
//         }
//         else if (!hasFoundPeek && autocorrelResult[i] > peekDetectorThreshold && autocorrelResult[i] > autocorrelResult[i-1])
//         {
//             hasFoundPeek = true;
//         }
//     }

//     return frequency;
// }

void AmpedUp::AudioUtil::printAudioData(sample_t* sampleData, uint32_t length)
{
    Serial.print("[");
    bool printSeparator = false;
    for (uint32_t i = 0; i < length; i++)
    {
        if (printSeparator)
        {
            Serial.print(", ");
        }
        Serial.print(sampleData[i]);
        printSeparator = true;
    }
    Serial.println("]");
}