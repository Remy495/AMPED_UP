
#include "NodeGraph.hpp"


AmpedUp::NodeGraph::NodeGraph() = default;

AmpedUp::NodeGraph::~NodeGraph() = default;

void AmpedUp::NodeGraph::clear()
{
capacity_ = 0;
currentSize_ = 0;
currentBufferEndIndex_ = 0;
outputNodeIndex_ = 0;
}

void AmpedUp::NodeGraph::setNodeCount(uint32_t nodeCount)
{
    capacity_ = nodeCount;
}

AmpedUp::Node& AmpedUp::NodeGraph::nodeAtIndex(uint32_t index)
{
    return *(nodePtrIndexPtr()[index]);
}

void AmpedUp::NodeGraph::makeConnection(size_t sourceNodeIndex, size_t sourceOutputIndex, size_t destNodeIndex, size_t destInputIndex)
{
    const NodeValue* outputPtr = nodeAtIndex(sourceNodeIndex).getOutputPtr(sourceOutputIndex);
    nodeAtIndex(destNodeIndex).input(destInputIndex).connectTo(outputPtr, sourceNodeIndex, sourceOutputIndex);
}

void AmpedUp::NodeGraph::makeFixedValue(size_t destNodeIndex, size_t destInputIndex, NodeValue value)
{
    nodeAtIndex(destNodeIndex).input(destInputIndex).setFixedValue(value);
}

void AmpedUp::NodeGraph::update()
{
    for(size_t i = 0; i < currentSize_; i++) {
        nodeAtIndex(i).update();
    }
}

void AmpedUp::NodeGraph::setOutputNode(size_t index)
{
    outputNodeIndex_ = index;
}

size_t AmpedUp::NodeGraph::getOutputNodeIndex() const
{
    return outputNodeIndex_;
}

AmpedUp::NodeValue AmpedUp::NodeGraph::getOutputValue(size_t outputIndex)
{
    return nodeAtIndex(outputNodeIndex_).getOutputValue(outputIndex);
}

AmpedUp::Node** AmpedUp::NodeGraph::nodePtrIndexPtr()
{
    return reinterpret_cast<AmpedUp::Node**>(buffer_);
}

uint8_t* AmpedUp::NodeGraph::nodeDataPtr()
{
    return buffer_ + capacity_ * sizeof(Node*);
}

const uint8_t* AmpedUp::NodeGraph::nodeDataPtr() const
{
    return buffer_ + capacity_ * sizeof(Node*);
}