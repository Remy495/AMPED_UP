
#include "NodeGraph.hpp"



///////////////////////////////////////////////////////
/// @brief Create an empty node graph
///
/// @param nodeCapacity The maximum number of nodes that can be added to this node grpah
/// @param nodeBufferSize The amount of space required to store all of this node graph's nodes
///////////////////////////////////////////////////////
AmpedUp::NodeGraph::NodeGraph(size_t nodeCapacity, size_t nodeBufferSize) {
    // Set the maximum node count
    maxNodeCount_ = nodeCapacity;

    // Create the runtime buffer based on the specified node buffer size and the size needed for the node pointer index
    runtimeBufferSize_ = nodeBufferSize + nodePtrIndexSize();
    runtimeBufferPtr_ = new char[runtimeBufferSize_];

    // Set the address of the next node to the start of the node buffer
    nextNodePtr_ = nodeBuffer();
}

///////////////////////////////////////////////////////
/// @brief Destructor
///////////////////////////////////////////////////////
AmpedUp::NodeGraph::~NodeGraph() {
    // If a runtime buffer has been allocated, delete it
    if (runtimeBufferPtr_ != nullptr) {
        delete[] runtimeBufferPtr_;
        runtimeBufferPtr_ = nullptr;
    }
}

///////////////////////////////////////////////////////
/// @brief Get a node in this node graph by its index
///
/// @param index the index of the node to retrieve
///////////////////////////////////////////////////////
AmpedUp::Node& AmpedUp::NodeGraph::getNode(size_t index) {
    return *(nodePtrIndex()[index]);
}

///////////////////////////////////////////////////////
/// @brief Get a node in this node graph by its index
///
/// @param index the index of the node to retrieve
///////////////////////////////////////////////////////
const AmpedUp::Node& AmpedUp::NodeGraph::getNode(size_t index) const{
    return *(nodePtrIndex()[index]);
}

void AmpedUp::NodeGraph::makeConnection(size_t sourceNodeIndex, size_t sourceOutputIndex, size_t destNodeIndex, size_t destInputIndex) {
    const NodeValue* outputPtr = getNode(sourceNodeIndex).getOutputPtr(sourceOutputIndex);
    getNode(destNodeIndex).input(destInputIndex).connectTo(outputPtr, sourceNodeIndex, sourceOutputIndex);
}

void AmpedUp::NodeGraph::makeFixedValue(size_t destNodeIndex, size_t destInputIndex, NodeValue value) {
    getNode(destNodeIndex).input(destInputIndex).setFixedValue(value);
}

///////////////////////////////////////////////////////
/// @brief Update all of the nodes in this node graph and recalculate the output values
///////////////////////////////////////////////////////
void AmpedUp::NodeGraph::update() {
    for(size_t i = 0; i < nodeCount_; i++) {
        getNode(i).update();
    }
}

///////////////////////////////////////////////////////
/// @brief Set a particular node in this node graph as the output node.
/// @details The outputs of the designated output node are the outputs of the node graph as a whole
///
/// @param index The index of the output node
///////////////////////////////////////////////////////
void AmpedUp::NodeGraph::setOutputNode(size_t index) {
    outputNodeIndex_ = index;
}

size_t AmpedUp::NodeGraph::getOutputNodeIndex() const {
    return outputNodeIndex_;
}

///////////////////////////////////////////////////////
/// @brief Get the number of outputs this node graph has
///////////////////////////////////////////////////////
size_t AmpedUp::NodeGraph::numOutputs() const {
    // If the output node index is valid, report its outputs; otherwise report no outputs
    return outputNodeIndex_ < nodeCount_ ? getNode(outputNodeIndex_).numOutputs() : 0;
}

///////////////////////////////////////////////////////
/// @brief Get the value of one of this node graph's outputs
///////////////////////////////////////////////////////
AmpedUp::NodeValue AmpedUp::NodeGraph::getOutputValue(size_t outputIndex) const {
    // If the output node index is valid, report its outputs; otherwise report a default value
    return outputNodeIndex_ < nodeCount_ ? getNode(outputNodeIndex_).getOutputValue(outputIndex) : 0;
}

///////////////////////////////////////////////////////
/// @brief Get a pointer to the node buffer
/// @details in order to reduce heap fragmentation, all of the nodes in a node graph are stored back-to-back in
///          one large buffer, as opposed to allocating memory for each node separately
///////////////////////////////////////////////////////
char* AmpedUp::NodeGraph::nodeBuffer() {
    // Node buffer is immediately after node pointer index in the runtime buffer
    return runtimeBufferPtr_ + nodePtrIndexSize();
}

///////////////////////////////////////////////////////
/// @brief Get a pointer to the node buffer
/// @details in order to reduce heap fragmentation, all of the nodes in a node graph are stored back-to-back in
///          one large buffer, as opposed to allocating memory for each node separately
///////////////////////////////////////////////////////
const char* AmpedUp::NodeGraph::nodeBuffer() const {
    // Node buffer is immediately after node pointer index in the runtime buffer
    return runtimeBufferPtr_ + nodePtrIndexSize();
}

///////////////////////////////////////////////////////
/// @brief Get the size (in bytes) of the node buffer
///////////////////////////////////////////////////////
size_t AmpedUp::NodeGraph::nodeBufferSize() const {
    return runtimeBufferSize_ - nodePtrIndexSize();
}

///////////////////////////////////////////////////////
/// @brief Get the amount of space (in bytes) within the node buffer that is currently occupied by nodes
///////////////////////////////////////////////////////
size_t AmpedUp::NodeGraph::nodeBufferSpaceUsed() const {
    return nextNodePtr_ - nodeBuffer();
}

///////////////////////////////////////////////////////
/// @brief Get the amount of free space (int bytes) withint the node buffer
///////////////////////////////////////////////////////
size_t AmpedUp::NodeGraph::nodeBufferSpaceRemaining() const {
    return nodeBufferSize() - nodeBufferSpaceUsed();
}

///////////////////////////////////////////////////////
/// @brief Get a pointer to the node index
/// @details Nodes in the node buffer are "disorganized" meaning that there is no regular spacing or in-band
///          metadata indicating where one node ends and the next begins. Thus it is necessary to keep an array
///          of pointers indicating the start of each node
///////////////////////////////////////////////////////
AmpedUp::Node** AmpedUp::NodeGraph::nodePtrIndex() {
    return reinterpret_cast<Node**>(runtimeBufferPtr_);
}

///////////////////////////////////////////////////////
/// @brief Get a pointer to the node index
/// @details Nodes in the node buffer are "disorganized" meaning that there is no regular spacing or in-band
///          metadata indicating where one node ends and the next begins. Thus it is necessary to keep an array
///          of pointers indicating the start of each node
///////////////////////////////////////////////////////
const AmpedUp::Node** AmpedUp::NodeGraph::nodePtrIndex() const {
    return reinterpret_cast<const Node**>(runtimeBufferPtr_);
}

///////////////////////////////////////////////////////
/// @brief Get the size (in bytes) of the node pointer index
///////////////////////////////////////////////////////
size_t AmpedUp::NodeGraph::nodePtrIndexSize() const {
    return sizeof(Node*) * maxNodeCount_;
}