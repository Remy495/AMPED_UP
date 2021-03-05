
#include "AmpedUpMessaging_generated.h"
#include "AmpedUpNodes_generated.h"

#include "AddNode.hpp"
#include "MultiplyNode.hpp"
#include "NodeGraph.hpp"

#include "Time.hpp"

#include "Arduino.h"

// #include "i2c_device.h"

#include "I2cMessagerServer.hpp"


int main(int argc, char** argv) {

    Serial.begin(9600);

    // I2CMaster& master = Master;
    // master.begin(400000);

    // uint32_t receivedValue{};
    // uint32_t sentValue{};

    // AmpedUp::I2cConfig<AmpedUp::I2cHandle::I2C_1> x;

    // while (true)
    // {
    //     master.write_async(8, (uint8_t*)&sentValue, 4, true);
    //     delay(500);
    //     master.read_async(8, (uint8_t*)&receivedValue, 4, true);
    //     delay(500);
    //     Serial.println(receivedValue);
    //     sentValue++;
    // }

    uint32_t receivedValue{};
    uint32_t transmittedValue{};

    delay(5000);
    Serial.println("connected");
    delay(1000);

    auto& i2cMessager = AmpedUp::I2cMessager<AmpedUp::I2cHandle::I2C_1, uint32_t, 2>::getInstance();

    i2cMessager.begin(AmpedUp::I2cSpeed::FAST_400K, 8);

    while (true)
    {
        if (i2cMessager.hasReceivedMessageFromIndex(0))
        {
            receivedValue = i2cMessager.takeReceivedMessageFromIndex(0);
            Serial.println(receivedValue);
            transmittedValue++;
            i2cMessager.sendToIndex(0, transmittedValue);
        }
    }



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