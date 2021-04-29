

#ifndef _INPUT_NODE_
#define _INPUT_NODE_

#include "Node.hpp"

namespace AmpedUp {

    ///////////////////////////////////////////////////////
    /// @brief Represents an abstract node with no inputs and a fixed number of outputs
    ///////////////////////////////////////////////////////
    template<size_t nOutputs>
    class InputNode: public Node {
    public:

        InputNode() = default;

        ///////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ///////////////////////////////////////////////////////
        virtual ~InputNode() = default;

        ///////////////////////////////////////////////////////
        /// @brief Get the number of outputs this node has
        ///////////////////////////////////////////////////////
        size_t numOutputs() const {
            return nOutputs;
        }

        ///////////////////////////////////////////////////////
        /// @brief Get the number of inputs this node has (0, as this is an input node)
        ///////////////////////////////////////////////////////
        size_t numInputs() const {
            return 0;
        }

    protected:
        ///////////////////////////////////////////////////////
        /// @brief Get this node's input array. Returns nullptr, as this node has no inputs
        ///////////////////////////////////////////////////////
        NodeInput* inputs() {
            return nullptr;
        }

        ///////////////////////////////////////////////////////
        /// @brief Get this node's output array
        ///////////////////////////////////////////////////////
        const NodeValue* outputs() const {
            return outputs_;
        }

        NodeValue outputs_[nOutputs];
    };
}

#endif