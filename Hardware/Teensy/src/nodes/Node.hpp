

#ifndef _NODE_
#define _NODE_

#include "AmpedUpNodes_generated.h"

namespace AmpedUp {

    using NodeValue = float;

    class NodeGraph;

    ///////////////////////////////////////////////////////
    /// @brief Represents and input to a node. Either holds a constant value or points to an output value of another
    ///        node
    ///////////////////////////////////////////////////////
    class NodeInput {
    public:

        ///////////////////////////////////////////////////////
        /// @brief Get the value currently held by this input
        ///////////////////////////////////////////////////////
        NodeValue getValue() const {
            return *valuePtr_;
        }

        ///////////////////////////////////////////////////////
        /// @brief Check whether this node input holds a fixed value or is connected to a node output
        ///////////////////////////////////////////////////////
        bool isFixedValue() const {
            return valuePtr_ == &fixedValue_;
        }

        ///////////////////////////////////////////////////////
        /// @brief Assign this node input a fixed value (and disconnect it from its output if it is connected to one)
        ///
        /// @param value The value to assign this node input
        ///////////////////////////////////////////////////////
        void setFixedValue(NodeValue value) {
            fixedValue_ = value;
            valuePtr_ = &fixedValue_;
        }

        ///////////////////////////////////////////////////////
        /// @brief Connect this node input to a node output value
        ///
        /// @param valuePtr A pointer to the output value to connect to
        /// @param connectedNodeIndex The index in the node graph of the node being connected to
        /// @param connectedOutputIndex The index on the of the specified output value on the node being connected to
        ///////////////////////////////////////////////////////
        void connectTo(const NodeValue* valuePtr, size_t connectedNodeIndex, size_t connectedOutputIndex) {
            valuePtr_ = valuePtr;
            connectedNodeIndex_ = connectedNodeIndex;
            connectedOutputIndex_ = connectedOutputIndex;
        }

    private:
        const NodeValue* valuePtr_{&fixedValue_};
        NodeValue fixedValue_{0.0f};
        size_t connectedNodeIndex_{0};
        size_t connectedOutputIndex_{0};
    };

    ///////////////////////////////////////////////////////
    /// @brief Represents an abstract node in a signal processing node graph.
    /// @details All node implementations must inherit from Node. However, most should not directly subclass Node, but
    ///          rather subclass InputNode, IntermediateNode, or OutputNode 
    ///////////////////////////////////////////////////////
    class Node {
    public:

        ///////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ///////////////////////////////////////////////////////
        virtual ~Node() = default;

        ///////////////////////////////////////////////////////
        /// @brief Recalculate the output values of the node based on its inputs
        ///////////////////////////////////////////////////////
        virtual void update() = 0;

        ///////////////////////////////////////////////////////
        /// @brief Get a the value of one of this node's outputs
        ///
        /// @param outputIndex the index of the output in question. Must be less than numOutputs(), otherwise undefined
        ///                    will occur.
        ///////////////////////////////////////////////////////
        NodeValue getOutputValue(size_t outputIndex) const {
            return outputs()[outputIndex];
        }

        ///////////////////////////////////////////////////////
        /// @brief Get a pointer to one of this node's output values
        ///
        /// @param outputIndex the index of the output in question. Must be less than numOutputs(), otherwise undefined
        ///                    will occur.
        ///////////////////////////////////////////////////////
        const NodeValue* getOutputPtr(size_t outputIndex) const {
            return outputs() + outputIndex;
        }

        ///////////////////////////////////////////////////////
        /// @brief Get a mutable reference to one of this node's inputs
        ///
        /// @param inputIndex the index of the input in question. Must be less than numInputs(), otherwise undefined
        ///                   will occur.
        ///////////////////////////////////////////////////////
        NodeInput& input(size_t inputIndex) {
            return inputs()[inputIndex];
        }

        ///////////////////////////////////////////////////////
        /// @brief Get the number of outputs this node has
        ///////////////////////////////////////////////////////
        virtual size_t numOutputs() const = 0;

        ///////////////////////////////////////////////////////
        /// @brief Get the number of inputs this node has
        ///////////////////////////////////////////////////////
        virtual size_t numInputs() const = 0;

        ///////////////////////////////////////////////////////
        /// @brief Get the node type for this node
        ///////////////////////////////////////////////////////
        AmpedUpNodes::NodeType getNodeType() const {
            return nodeType_;
        }

        void setOwner(NodeGraph* owner)
        {
            ownerPtr_ = owner;
        }

    protected:
        AmpedUpNodes::NodeType nodeType_;
        NodeGraph* ownerPtr_{nullptr};


        ///////////////////////////////////////////////////////
        /// @brief Get this node's input array; to be implemented by subclasses
        ///////////////////////////////////////////////////////
        virtual NodeInput* inputs() = 0;

        ///////////////////////////////////////////////////////
        /// @brief Get this node's output array; to be implemented by subclasses
        ///////////////////////////////////////////////////////
        virtual const NodeValue* outputs() const = 0;
    };

}

#endif