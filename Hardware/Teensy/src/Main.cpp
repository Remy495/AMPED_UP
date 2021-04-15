
#include "AmpedUpMessaging_generated.h"
#include "AmpedUpNodes_generated.h"

#include "AddNode.hpp"
#include "MultiplyNode.hpp"
#include "NodeGraph.hpp"

#include "Time.hpp"

#include "Arduino.h"

// #include "i2c_device.h"

#include "I2cMessagerServer.hpp"
#include "KnobMessage.hxx"
#include "ConstantsCommon.hpp"

#include "ADC.hxx"

#include <cmath>

#include "CheckedMessage.h"
#include "RemoteMessageHeader.hpp"
#include "BinaryUtil.hxx"
// #include "SPI.h"

#include <cstring>

#include "SpiMessager.hpp"
#include "SpiInterface.hpp"

#include "TextLogging.hxx"

void externalInterruptHandler()
{
    // Serial.println("Rising edge!");
    // GPIO6_ISR = 1 << 3;
}


int main(int argc, char** argv) {

    AmpedUp::TextLogging::initialize();
    AmpedUp::TextLogging::enableDebugVerbosity();
    AmpedUp::TextLogging::enableInfoVerbosity();
    AmpedUp::TextLogging::enableWarningVerbosity();
    AmpedUp::TextLogging::enableCriticalVerbosity();
    AmpedUp::TextLogging::enableFatalVerbosity();
    

    AmpedUp::Time::initialize();

    // GPIO6_ICR1 = 0x2 << 6;
    // GPIO6_IMR = 1 << 3;

    // attachInterruptVector(IRQ_GPIO6789, externalInterruptHandler);
    // NVIC_ENABLE_IRQ(IRQ_GPIO6789);
    


    AmpedUp::SpiMessager::begin();


   
    while (1)
    {
        AmpedUp::SpiMessager::beginTransaction();
        delay(100);

        if (AmpedUp::SpiMessager::hasRecievedMessage())
        {
            const auto& recievedPayload = AmpedUp::SpiMessager::peekRecievedMessage();
 
            Serial.print("recieved message ");
            Serial.print(recievedPayload.getUsedSize());
            Serial.print(" ");
            const char* str = reinterpret_cast<const char*>(recievedPayload.data());
            for (uint32_t i = 0; i < recievedPayload.getUsedSize(); i++)
            {
                Serial.print(str[i]);
            }
            Serial.println("");


            if (AmpedUp::SpiMessager::isReadyToSend())
            {
                auto& outgoingPayload = AmpedUp::SpiMessager::stageOutgoingMessage();
                memcpy(outgoingPayload.data(), recievedPayload.data(), recievedPayload.getUsedSize());
                outgoingPayload.setUsedSize(recievedPayload.getUsedSize());
                AmpedUp::SpiMessager::sendStagedMessage();

            }

            AmpedUp::SpiMessager::discardRecievedMessage();
        }
    }
    

    // AmpedUp::SpiInterface::enable();
    // while (1)
    // {

    //     AmpedUp::SpiInterface::beginTransaction();

    //     for(uint32_t i = 0; i < 200; i++)
    //     {

    //         while(!AmpedUp::SpiInterface::canTransmitData());
    //         AmpedUp::SpiInterface::sendWord(i);

    //         while (AmpedUp::SpiInterface::canReceiveData())
    //         {
    //             AmpedUp::SpiInterface::recieveWord();
    //         }
    //     }
    //     while(!AmpedUp::SpiInterface::canTransmitData());
    //     AmpedUp::SpiInterface::endTransaction();
    //     while ((LPSPI4_RSR & LPSPI_RSR_RXEMPTY))
    //     (void)(LPSPI4_RDR);
    //     Serial.println(tempmonGetTemp());


    //     delay(1000);
    // }

    // pinMode(SS, OUTPUT);
    // SPI.begin();

    // const char message[] = "This is a test message..";
    // CheckedMessage<AmpedUp::RemoteMessageHeader> headerChecked(AmpedUp::RemoteMessageHeader(AmpedUp::RemoteMessageType::REMOTE_MESSAGE, strlen(message)));
    // const char* headerData = reinterpret_cast<const char*>(&headerChecked);
    // uint32_t headerSize = AmpedUp::BinaryUtil::bytesFillWords(sizeof(headerChecked));

    // while(1)
    // {
    //     spiTransaction(headerData, headerSize);
    //     delay(500);
    //     spiTransaction(message, strlen(message));
    //     delay(500);
    // }

    // CCM_CCGR1 |= CCM_CCGR1_ADC1(CCM_CCGR_ON);
    // ADC1_CFG = ADC_CFG_OVWREN | ADC_CFG_MODE(1) | ADC_CFG_ADSTS(2) | ADC_CFG_ADLSMP;
    // ADC1_GC = ADC_GC_ADCO | ADC_GC_CAL;

	// while (ADC1_GC & ADC_GC_CAL) ;

    

    // AmpedUp::Adc::begin(1);


    // while (true)
    // {
    //     for (int i = 0; i < 44100; i++)
    //     {
	// 	    while
    //          (!(ADC1_HS & ADC_HS_COCO0)) ; // wait
    //         sampledValue = AmpedUp::Adc::getCurrentValue();
    //     }
    //     Serial.print(AmpedUp::Time::getSeconds());
    //     Serial.print(": ");
    //     Serial.println(sampledValue);
    // }


    // Serial.begin(9600);
    // AmpedUp::Time::initialize();


    // auto& i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, AmpedUp::KnobMessage, 1>::getInstance();
    // i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);

    // float lastPrintTime = 0;
    // constexpr float printInterval = 0.1f;
    // float currentKnobPosition = 0.0f;
    // while (true)
    // {
    //     float currentTime = AmpedUp::Time::getSeconds();
    //     float knobValue = (std::sin(currentTime) + 1) / 2;
    //     AmpedUp::KnobMessage outgoingMessage(AmpedUp::KnobMessageType::SET_KNOB_POSITION, knobValue);
    //     i2cMessager.sendToIndex(0, outgoingMessage);

    //     if (i2cMessager.hasReceivedMessageFromIndex(0))
    //     {
    //         AmpedUp::KnobMessage incomingMessage = i2cMessager.takeReceivedMessageFromIndex(0);
    //         currentKnobPosition = incomingMessage.getValue();
    //     }
        

    //     if (currentTime > lastPrintTime + printInterval)
    //     {
    //         Serial.println(currentKnobPosition);
    //         lastPrintTime = currentTime;
    //     }
    // }

    // auto i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, uint8_t, AmpedUp::Constants::DAUGHTER_BOARD_COUNT>::getInstance();
    // i2cMessager.begin(AmpedUp::I2cSpeed::STANDARD_100K, AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);

    // pinMode(13, OUTPUT);

    // float lastToggleTime = 0;
    // float toggleInterval = 2.0f / AmpedUp::Constants::DAUGHTER_BOARD_COUNT;
    // uint32_t daughterBoardIndex = 0;
    // uint8_t message = 0;
    
    // while(true)
    // {
    //     float currentTime = AmpedUp::Time::getSeconds();

    //     if (currentTime > lastToggleTime + toggleInterval)
    //     {
    //         lastToggleTime = currentTime;
    //         i2cMessager.sendToIndex(daughterBoardIndex, message);

    //         daughterBoardIndex++;
    //         if (daughterBoardIndex >= AmpedUp::Constants::DAUGHTER_BOARD_COUNT)
    //         {
    //             daughterBoardIndex = 0;
    //             message = ~message;
    //         }
    //     }

    //     i2cMessager.sendToIndex(0, message);
    // }

    // size_t nodeCount = 3;

    
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