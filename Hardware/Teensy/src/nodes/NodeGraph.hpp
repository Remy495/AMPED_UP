
#ifndef _NODE_GRAPH_
#define _NODE_GRAPH_

#include <cstring>
#include <cstdint>
#include <type_traits>
#include <iostream>

#include "BinaryUtil.hxx"
#include "NodeGraphContext.hpp"
#include "Node.hpp"

#include "Arduino.h"

namespace AmpedUp {
    
    class NodeGraph {
    public:

        NodeGraph();

        ~NodeGraph();

        void clear();

        void setNodeCount(uint32_t nodeCount);

        template<typename T>
        void addNode(const T& node)
        {
            // Find the address to store the node at
            uint32_t bufferOffset = BinaryUtil::bytesFillWords(currentBufferEndIndex_);
            Node* newNodeAddress = reinterpret_cast<Node*>(nodeDataPtr() + bufferOffset);

            // Copy the node there
            new (newNodeAddress) T(node);
            newNodeAddress->setOwner(this);
            // Write the node's address to the index
            nodePtrIndexPtr()[currentSize_] = newNodeAddress;

            // Advance the relevant parameters
            currentSize_++;
            currentBufferEndIndex_ = bufferOffset + sizeof(T);
        }

        Node& nodeAtIndex(uint32_t index);

        void makeConnection(size_t sourceNodeIndex, size_t sourceOutputIndex, size_t destNodeIndex, size_t destInputIndex);

        void makeFixedValue(size_t destNodeIndex, size_t destInputIndex, NodeValue value);

        void update();

        void setOutputNode(size_t index);

        size_t getOutputNodeIndex() const;

        NodeValue getOutputValue(size_t outputIndex);

        void setContext(const NodeGraphContext& context)
        {
            context_ = context;
        }

        const NodeGraphContext& getContext() const
        {
            return context_;
        }

    private:

        NodeGraphContext context_;

        static constexpr uint32_t BUFFER_SIZE = 16384;
        alignas(uint32_t) uint8_t buffer_[BUFFER_SIZE];

        uint32_t capacity_{};
        uint32_t currentSize_{};
        uint32_t currentBufferEndIndex_{};
        uint32_t outputNodeIndex_{};

        Node** nodePtrIndexPtr();

        uint8_t* nodeDataPtr();

        const uint8_t* nodeDataPtr() const;

    };
}

#endif