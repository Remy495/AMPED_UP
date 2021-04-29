
#include "AmpedUpMessaging_generated.h"
#include "AmpedUpNodes_generated.h"

#include "AddNode.hpp"
#include "MultiplyNode.hpp"
#include "KnobValuesNode.hpp"
#include "NodeGraph.hpp"
#include "GuitarSignalNode.hpp"
#include "RemapRangeNode.hpp"
#include "SinWaveNode.hpp"
#include "ExpressionPedalNode.hpp"
#include "AverageNode.hpp"
#include "ReverseNode.hpp"

#include "Time.hpp"

#include "Arduino.h"

// #include "i2c_device.h"

#include "I2cMessagerServer.hpp"
#include "KnobMessage.hxx"
#include "ConstantsCommon.hpp"

#include "ADC.hxx"

#include <cmath>
#include <algorithm>

#include "CheckedMessage.h"
#include "RemoteMessageHeader.hpp"
#include "BinaryUtil.hxx"
// #include "SPI.h"

#include <cstring>

#include "SpiMessager.hpp"
#include "SpiInterface.hpp"

#include "TextLogging.hxx"

#include "RemoteCommunication.hpp"

#include "arm_math.h"

#include "SlidingWindow.hxx"
#include "WindowSampler.hpp"

volatile int sampleCounter;

AmpedUp::NodeGraph nodeGraph;

bool printAudio = false;;

int main(int argc, char** argv) {

    AmpedUp::TextLogging::initialize();
    // AmpedUp::TextLogging::enableDebugVerbosity();
    AmpedUp::TextLogging::enableInfoVerbosity();
    AmpedUp::TextLogging::enableWarningVerbosity();
    AmpedUp::TextLogging::enableCriticalVerbosity();
    AmpedUp::TextLogging::enableFatalVerbosity();
    

    AmpedUp::Time::initialize();

    AmpedUp::NodeGraph nodeGraph;
    nodeGraph.clear();
    nodeGraph.setNodeCount(1);
    nodeGraph.addNode(AmpedUp::KnobValuesNode());
    nodeGraph.makeFixedValue(0, 0, 0.5);
    nodeGraph.makeFixedValue(0, 1, 0.5);
    nodeGraph.makeFixedValue(0, 2, 0.5);
    nodeGraph.makeFixedValue(0, 3, 0.5);
    nodeGraph.makeFixedValue(0, 4, 0.5);
    nodeGraph.makeFixedValue(0, 5, 0.5);
    nodeGraph.makeFixedValue(0, 6, 0.5);
    nodeGraph.makeFixedValue(0, 7, 0.5);
    nodeGraph.setOutputNode(0);

    AmpedUp::Time lastPrintTime;
    AmpedUp::RemoteCommunication::initialize(3);
    AmpedUp::WindowSampler::initialize(1, AmpedUp::AudioUtil::SAMPLE_RATE_HZ);

    auto& i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, AmpedUp::KnobMessage, AmpedUp::Constants::DAUGHTER_BOARD_COUNT>::getInstance();
    i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);


    bool graphHasAudio = false;

    int countsPerSecond = 0;

    AmpedUp::SlidingWindow<float, 8> smoothedPitch;
    AmpedUp::SlidingWindow<float, 8> smoothedVolume;

    AmpedUp::AudioUtil::init();

    // pinMode(3, INPUT);

    const float PITCH_LOG_MAX = std::log2f(AmpedUp::AudioUtil::MAXIMUM_FREQUENCY_HZ);
    const float PITCH_LOG_MIN = std::log2f(AmpedUp::AudioUtil::MINIMUM_FREQUENCY_HZ);

    // while (true)
    // {
    //     AmpedUp::WindowSampler::freezeWindow();

    //     auto* data = AmpedUp::WindowSampler::getCurrentWindow();

    //     auto [currentVolume, currentPitch] = AmpedUp::AudioUtil::analyze(data);

    //     currentPitch = std::log2f(currentPitch);
    //     currentPitch = (currentPitch - PITCH_LOG_MIN) / (PITCH_LOG_MAX - PITCH_LOG_MIN);

    //     AmpedUp::WindowSampler::unfreezeWindow();

    //     smoothedVolume.append(currentVolume);
    //     if (currentVolume > 0.05 && currentPitch > 0)
    //     {
    //         smoothedPitch.append(currentPitch);
    //     }

    //     float volume;
    //     float pitch;

    //     arm_mean_f32(smoothedPitch.getCurrentWindow(), smoothedPitch.getCurrentWindowOccupancy(), &pitch);
    //     arm_mean_f32(smoothedVolume.getCurrentWindow(), smoothedVolume.getCurrentWindowOccupancy(), &volume);

    //     // printAudio = !digitalRead(3);

    //     countsPerSecond++;

    //     if (printAudio)
    //     {
    //         Serial.println("data");
    //         AmpedUp::AudioUtil::printAudioData(data, AmpedUp::AudioUtil::WINDOW_SIZE);
    //     }
    //     else
    //     {
    //         Serial.print(100 * volume);
    //         Serial.print(" ");
    //         Serial.println(100 * pitch);
    //     }

    //     // if (AmpedUp::Time::now() > lastPrintTime + 1_s)
    //     // {
    //     //     Serial.println(countsPerSecond);
    //     //     lastPrintTime = AmpedUp::Time::now();
    //     //     countsPerSecond = 0;
    //     // }

    // }

    while (1)
    {
        // Check for new messages from the remote GUI
        if (AmpedUp::RemoteCommunication::hasRecievedMessage())
        {
            const auto* incomingMessagePtr = AmpedUp::RemoteCommunication::getRecievedMessage();
            if (incomingMessagePtr->payload_type() == AmpedUpMessaging::MessagePayload::SavePresetMessage)
            {
                AmpedUp::TextLogging::info(__FILE__, __LINE__, "Recieved new node graph");

                auto* savePresetMessagePtr = incomingMessagePtr->payload_as_SavePresetMessage();
                auto* nodeGraphData = savePresetMessagePtr->value();

                // Load the node graph
                nodeGraph.clear();

                graphHasAudio = false;
                
                auto* nodeVector = nodeGraphData->nodes();
                nodeGraph.setNodeCount(nodeVector->size());
                for (uint32_t i = 0; i < nodeVector->size(); i++)
                {   
                    // Add the node to the graph
                    const auto* currentNode = nodeVector->Get(i);
                    // AmpedUp::TextLogging::info(__FILE__, __LINE__, AmpedUpNodes::EnumNameNodeType(currentNode->type()));
                    switch (currentNode->type())
                    {
                    case AmpedUpNodes::NodeType::ADD:
                        nodeGraph.addNode(AmpedUp::AddNode());
                        break;
                    case AmpedUpNodes::NodeType::MULTIPLY:
                        nodeGraph.addNode(AmpedUp::MultiplyNode());
                        break;
                    case AmpedUpNodes::NodeType::REVERSE:
                        nodeGraph.addNode(AmpedUp::ReverseNode());
                        break;
                    case AmpedUpNodes::NodeType::AVERAGE:
                        nodeGraph.addNode(AmpedUp::AverageNode());
                        break;
                    case AmpedUpNodes::NodeType::KNOB_POSITIONS:
                    {
                        AmpedUp::KnobValuesNode knobValuesNode;

                        // Set the output ranges for the knobs
                        const auto* outputRangesVector = nodeGraphData->outputRanges();
                        for (uint32_t j = 0; j < outputRangesVector->size(); j++)
                        {
                            const auto* currentOutputRange = outputRangesVector->Get(j);
                            knobValuesNode.setKnobMin(j, currentOutputRange->minimum());
                            knobValuesNode.setKnobMax(j, currentOutputRange->maximum());
                        }

                        nodeGraph.addNode(knobValuesNode);
                        nodeGraph.setOutputNode(i);
                        break;
                    }
                    case AmpedUpNodes::NodeType::GUITAR_SIGNAL:
                        nodeGraph.addNode(AmpedUp::GuitarSignalNode());
                        graphHasAudio = true;
                        break;
                    case AmpedUpNodes::NodeType::REMAP_RANGE:
                        nodeGraph.addNode(AmpedUp::RemapRangeNode());
                        break;
                    case AmpedUpNodes::NodeType::SINE_WAVE:
                        nodeGraph.addNode(AmpedUp::SineWaveNode());
                        break;
                    case AmpedUpNodes::NodeType::EFFECTS_PEDAL:
                        nodeGraph.addNode(AmpedUp::ExpressionPedalNode());
                        break;
                    default:
                        AmpedUp::TextLogging::critical(__FILE__, __LINE__, "Unsupported node type");
                        break;
                    }

                    // Add the connections to the node
                    const auto* inputsPtr = currentNode->inputs();
                    for (uint32_t j = 0; j < inputsPtr->size(); j++)
                    {
                        const auto* input = inputsPtr->Get(j);
                        if (input->value_type() == AmpedUpNodes::InputValue::Connection)
                        {
                            const auto* connection = input->value_as_Connection();
                            nodeGraph.makeConnection(connection->nodeIndex(), connection->outputIndex(), i, j);
                        }
                        else
                        {
                            const auto* literalValue = input->value_as_ConstantValueFloat();
                            nodeGraph.makeFixedValue(i, j, literalValue->value());
                        }
                        
                    }
                }
            }
            else
            {
                AmpedUp::TextLogging::critical(__FILE__, __LINE__, "Wrong message type");
            }

            AmpedUp::RemoteCommunication::discardRecievedMessage();
        }


        AmpedUp::NodeGraphContext currentContext;
        AmpedUp::NodeGraphContext prevContext = nodeGraph.getContext();

        if (graphHasAudio)
        {
            AmpedUp::WindowSampler::freezeWindow();

            auto* data = AmpedUp::WindowSampler::getCurrentWindow();

            auto [currentVolume, currentPitch] = AmpedUp::AudioUtil::analyze(data);

            currentPitch = std::log2f(currentPitch);
            currentPitch = (currentPitch - PITCH_LOG_MIN) / (PITCH_LOG_MAX - PITCH_LOG_MIN);

            AmpedUp::WindowSampler::unfreezeWindow();

            smoothedVolume.append(currentVolume);
            if (currentVolume > 0.05 && currentPitch > 0)
            {
                smoothedPitch.append(currentPitch);
            }

            arm_mean_f32(smoothedPitch.getCurrentWindow(), smoothedPitch.getCurrentWindowOccupancy(), &currentContext.guitarPitch);
            arm_mean_f32(smoothedVolume.getCurrentWindow(), smoothedVolume.getCurrentWindowOccupancy(), &currentContext.guitarVolume);
        }

        AmpedUp::WindowSampler::freezePedal();
        auto* pedalWindow = AmpedUp::WindowSampler::getPedalWindow();
        float pedalInputAverage;
        arm_mean_f32(pedalWindow, 1200, &pedalInputAverage);
        AmpedUp::WindowSampler::unfreezePedal();

        const float PEDAL_MIN = std::log2f(0.1);
        const float PEDAL_MAX = std::log2f(0.61);

        float pedalPos = (std::log2f(pedalInputAverage) - PEDAL_MIN) / (PEDAL_MAX - PEDAL_MIN);
        pedalPos = std::min(std::max(pedalPos, 0.0f), 1.0f);

        currentContext.expressionPedalPosition = pedalPos;


        currentContext.currentTime = AmpedUp::Time::now();
        currentContext.cycleElapsedTime = currentContext.currentTime - prevContext.currentTime;

        nodeGraph.setContext(currentContext);

        nodeGraph.update();

        for (int i = 0; i < 8; i++)
        {
            AmpedUp::KnobMessage outgoingMessage(AmpedUp::KnobMessageType::SET_KNOB_POSITION, nodeGraph.getOutputValue(i));
            i2cMessager.sendToIndex(i, outgoingMessage);
        }

        if (AmpedUp::Time::now() > lastPrintTime + 1_s)
        {
            lastPrintTime = AmpedUp::Time::now();
            AmpedUp::TextLogging::info(__FILE__, __LINE__, nodeGraph.getOutputValue(0));
            for (uint8_t i = 0; i < 8; i++)
            {
                Serial.println(i2cMessager.isSlaveOnline(i));
            }
            Serial.println(pedalInputAverage);
        }
    }

    // float inputData[5000];
    // float inputDataReverse[5000];
    // float outputData[5000];

    // int i = 0;
    // AmpedUp::Time lastPrintTime = AmpedUp::Time::now();
    // while (true)
    // {
    //     for (int i = 0; i < 5000; i++)
    //     {
    //         inputDataReverse[i] = inputData[4999 - i];
    //     }

    //     arm_conv_partial_f32(inputData, 5000, inputData, 5000, outputData, 2500, 800);

    //     i++;
    //     if (AmpedUp::Time::now() >= lastPrintTime + 1_s)
    //     {
    //         Serial.println(i);
    //         i = 0;
    //         lastPrintTime = AmpedUp::Time::now();
    //     }
    // }

    // AmpedUp::Time interval = AmpedUp::Time::seconds(0.5);

    // PIT_LDVAL0 = interval.getTicks();
    // PIT_TCTRL0 |= PIT_TCTRL_TEN | PIT_TCTRL_TIE;

    // attachInterruptVector(IRQ_PIT, timerInterrupt);
    // NVIC_ENABLE_IRQ(IRQ_PIT);

    // while(1);

    // AmpedUp::WindowSampler::initialize(1, 44100.0f);

    // AmpedUp::Time lastPrintTime;
    // int itersPerSec = 0;
    // while(1)
    // {
    //     AmpedUp::WindowSampler::freezeWindow();

    //     auto* data = AmpedUp::WindowSampler::getCurrentWindow();

    //     float volume = AmpedUp::AudioUtil::findVolume(data);
    //     float pitch = AmpedUp::AudioUtil::findPitch(data);

    //     AmpedUp::WindowSampler::unfreezeWindow();
    //     itersPerSec++;

    //     if (AmpedUp::Time::now() > lastPrintTime + 1_s)
    //     {
    //         lastPrintTime = AmpedUp::Time::now();
    //         // AmpedUp::TextLogging::info(__FILE__, __LINE__, "volume: ", volume, " pitch: ", pitch);
    //         // AmpedUp::TextLogging::info(__FILE__, __LINE__, itersPerSec);
    //         Serial.println(itersPerSec);
    //         itersPerSec = 0;
    //     }
    // }


    // // calibrate ADC
	// ADC1_GC = ADC_GC_CAL;
	// while (ADC1_GC & ADC_GC_CAL);

    // AmpedUp::Time lastPrintTime;
    // AmpedUp::SlidingWindow<int16_t, 5000> values;
    // float knobPosition;
    // ADC1_HC0 = 2;

    // auto& i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, AmpedUp::KnobMessage, AmpedUp::Constants::DAUGHTER_BOARD_COUNT>::getInstance();
    // i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);


    // AmpedUp::SlidingWindow<float, 3> values;

    // while(1)
    // {
    //     // // Sample the pedal
    //     // while (!(ADC1_HS & ADC_HS_COCO0));
    //     // int16_t value = ADC1_R0;
    //     // ADC1_HC0 = 2;
    //     // values.append(value);

    //     // // Calculate the knob position
    //     // int16_t smoothedValue;
    //     // arm_mean_q15(values.getCurrentWindow(), 1000, &smoothedValue);
    //     // knobPosition = pedalToKnob(smoothedValue);

    //     values.append(getKnobPositionPitch());
    //     float knobPosition;

    //     arm_mean_f32(values.getCurrentWindow(), 3, &knobPosition);

    //     AmpedUp::KnobMessage outgoingMessage(AmpedUp::KnobMessageType::SET_KNOB_POSITION, knobPosition);
    //     i2cMessager.sendToIndex(0, outgoingMessage);

    //     if (AmpedUp::Time::now() > lastPrintTime + 0.5_s)
    //     {
    //         lastPrintTime = AmpedUp::Time::now();
    //         // AmpedUp::TextLogging::info(__FILE__, __LINE__, knobPosition);
    //         // Serial.println(knobPosition);
    //     }
    // }

    // // analog_init();

    // // set up interrupt
    // attachInterruptVector(IRQ_ADC1, adcInterrupt);
    // NVIC_ENABLE_IRQ(IRQ_ADC1);

    // // Begin first conversion
    // ADC1_HC0 = 1 | ADC_HC_AIEN;


    // while (1)
    // {
    //     Serial.print(i);
    //     Serial.print(" ");
    //     Serial.println(value);
    //     AmpedUp::Time::delay(1_s);
    // }

    // AmpedUp::RemoteCommunication::initialize(3);

    // while (1)
    // {
    //     // Wait for remote to connect
    //     while(!AmpedUp::RemoteCommunication::remoteIsConnected());

    //     AmpedUp::TextLogging::info(__FILE__, __LINE__, "This is log message from the main source file, logged using TextLogging");

    //     AmpedUp::Time::delay(1_s);
    // }
    // Serial.begin(9600);
    // AmpedUp::Time::initialize();


    // auto& i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, AmpedUp::KnobMessage, AmpedUp::Constants::DAUGHTER_BOARD_COUNT>::getInstance();
    // i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);


    // float lastTimeSeconds = AmpedUp::Time::now().getSeconds();

    // float currentKnobPositions[AmpedUp::Constants::DAUGHTER_BOARD_COUNT];
    // float currentKnobTargets[AmpedUp::Constants::DAUGHTER_BOARD_COUNT];

    // while (true)
    // {
    //     AmpedUp::Time currentTime = AmpedUp::Time::now();
    //     for (uint8_t i = 0; i < AmpedUp::Constants::DAUGHTER_BOARD_COUNT; i++)
    //     {
    //         float sinusoid = (std::sin(2*currentTime.getSeconds() + i * (6.28 / AmpedUp::Constants::DAUGHTER_BOARD_COUNT)) + 1) / 2;
    //         float knobValue = sinusoid * 0.9 + 0.05;
    //         currentKnobTargets[i] = knobValue;
    //         AmpedUp::KnobMessage outgoingMessage(AmpedUp::KnobMessageType::SET_KNOB_POSITION, knobValue);
    //         i2cMessager.sendToIndex(i, outgoingMessage);

    //         if (i2cMessager.hasReceivedMessageFromIndex(i))
    //         {
    //             AmpedUp::KnobMessage incomingMessage = i2cMessager.takeReceivedMessageFromIndex(i);
    //             if (incomingMessage.getMessageType() == AmpedUp::KnobMessageType::ERROR_COUNT)
    //             {
    //                 i2cMessager.setTransmitErrorCountForIndex(i, incomingMessage.getValueInt());
    //             }
    //             else
    //             {
    //                 currentKnobPositions[i] = i2cMessager.takeReceivedMessageFromIndex(i).getValueFloat();
    //             }
    //         }
    //     }

        
    //     if (AmpedUp::Time::now().getSeconds() > lastTimeSeconds + 0.1)
    //     {

    //         lastTimeSeconds = AmpedUp::Time::now().getSeconds();

    //         // Serial.println(outgoingMessage.getValueFloat());
    //         for (int i = 0; i < AmpedUp::Constants::DAUGHTER_BOARD_COUNT; i++)
    //         {
    //             Serial.print(currentKnobTargets[i]);
    //             if (i2cMessager.isSlaveOnline(i))
    //             {
    //                 Serial.print("_");
    //             }
    //             Serial.print(" ");
    //         }
    //         Serial.println("");
    //     }
    // }

    // auto i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, uint8_t, AmpedUp::Constants::DAUGHTER_BOARD_COUNT>::getInstance();
    // i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);

    
    // size_t nodeGraphSize = 2 * sizeof(AmpedUp::AddNode) + sizeof(AmpedUp::MultiplyNode);

    // AmpedUp::NodeGraph nodeGraph(nodeCount, nodeGraphSize);

    // nodeGraph.addNode(AmpedUp::AddNode());
    // nodeGraph.makeFixedValue(0, 0, 0.5);
    // nodeGraph.makeFixedValue(0, 1, 0.1);

    // nodeGraph.addNode(AmpedUp::MultiplyNode());
    // nodeGraph.makeConnection(0, 0, 1, 0);
    // nodeGraph.makeFixedValue(1, 1, 2);

    // nodeGraph.addNode(AmpedUp::AddNode());
    // nodeGraph.makeConnection(1, 0, 2, 1);
    // nodeGraph.makeConnection(0, 0, 2, 0);

    // nodeGraph.setOutputNode(2);

}