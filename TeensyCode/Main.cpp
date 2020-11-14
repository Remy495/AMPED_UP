
#include "AmpedUpMessaging_generated.h"
#include "math.h"
#include "AddNode.hpp"
#include "MultiplyNode.hpp"
#include "NodeGraph.hpp"

#include <iostream>


int main(int argc, char** argv) {

    // float a = 0.5;
    // float b = 0.6;

    // if (fmax(a, b) > 0.4) {

    //     //Create Node Buffer

    //     flatbuffers::FlatBufferBuilder builder(1024);

    //     auto firstInput = AmpedUpNodes::CreateConnection(builder, 0, 1);
    //     auto secondInput = AmpedUpNodes::CreateConstantValueFloat(builder, 0.5);

    //     flatbuffers::Offset<AmpedUpNodes::Input> inputs[2];

    //     inputs[0] = AmpedUpNodes::CreateInput(builder, AmpedUpNodes::InputValue::Connection, firstInput.Union());
    //     inputs[1] = AmpedUpNodes::CreateInput(builder, AmpedUpNodes::InputValue::ConstantValueFloat, secondInput.Union());

    //     auto inputsVec = builder.CreateVector(inputs, 2);

    //     auto node = AmpedUpNodes::CreateNode(builder, AmpedUpNodes::NodeTypes::ADD, inputsVec);

    //     builder.Finish(node);

    //     unsigned char* buffer = builder.GetBufferPointer();
    //     int size = builder.GetSize();

    //     // Read Node Buffer
    //     auto readNode = AmpedUpNodes::GetNode(buffer);

    //     auto readFirstInput = readNode->inputs()->Get(0);
    //     auto readFirstInputType = readFirstInput->value_type();


    //     std::cout << AmpedUpNodes::EnumNamesInputValue()[static_cast<int>(readFirstInputType)] << std::endl;
    // }

    size_t nodeCount = 3;
    size_t nodeGraphSize = 2 * sizeof(AmpedUp::AddNode) + sizeof(AmpedUp::MultiplyNode);

    AmpedUp::NodeGraph nodeGraph(nodeCount, nodeGraphSize);

    nodeGraph.addNode(AmpedUp::AddNode());
    nodeGraph.makeFixedValue(0, 0, 0.5);
    nodeGraph.makeFixedValue(0, 1, 0.1);

    nodeGraph.addNode(AmpedUp::MultiplyNode());
    nodeGraph.makeConnection(0, 0, 1, 0);
    nodeGraph.makeFixedValue(1, 1, 2);

    nodeGraph.addNode(AmpedUp::AddNode());
    nodeGraph.makeConnection(0, 0, 2, 0);
    nodeGraph.makeConnection(1, 0, 2, 1);

    nodeGraph.setOutputNode(2);

    nodeGraph.update();

    std::cout << nodeGraph.getOutputValue(0) << std::endl;
    
    return 0;
}