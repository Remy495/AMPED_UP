
#include "AmpedUpMessaging_generated.h"
#include "AmpedUpNodes_generated.h"

#include "AddNode.hpp"
#include "MultiplyNode.hpp"
#include "NodeGraph.hpp"

#include "Time.hpp"

#include "Arduino.h"

#include "i2c_device.h"


int main(int argc, char** argv) {

    Serial.begin(9600);

    AmpedUp::Time::initialize();

    I2CMaster& master = Master;
    master.begin(400000);

    uint32_t receivedValue{};
    uint32_t sentValue{};

    while (true)
    {
        master.write_async(8, (uint8_t*)&sentValue, 4, true);
        delay(500);
        master.read_async(8, (uint8_t*)&receivedValue, 4, true);
        delay(500);
        Serial.println(receivedValue);
        sentValue++;
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