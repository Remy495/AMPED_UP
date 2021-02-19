

#ifndef _INTERMEDIATE_NODE_
#define _INTERMEDIATE_NODE_

#include "Node.hpp"

namespace AmpedUp {

    template<size_t nInputs, size_t nOutputs>
    class IntermediateNode: public Node {
    public:

        ///////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ///////////////////////////////////////////////////////
        virtual ~IntermediateNode() = default;

        ///////////////////////////////////////////////////////
        /// @brief Get the number of outputs this node has
        ///////////////////////////////////////////////////////
        size_t numOutputs() const {
            return nOutputs;
        }

        ///////////////////////////////////////////////////////
        /// @brief Get the number of inputs this node has
        ///////////////////////////////////////////////////////
        size_t numInputs() const {
            return nInputs;
        }

    protected:
        ///////////////////////////////////////////////////////
        /// @brief Get this node's input array
        ///////////////////////////////////////////////////////
        NodeInput* inputs() {
            return inputs_;
        }

        ///////////////////////////////////////////////////////
        /// @brief Get this node's output array
        ///////////////////////////////////////////////////////
        const NodeValue* outputs() const {
            return outputs_;
        }

        NodeInput inputs_[nInputs];
        NodeValue outputs_[nOutputs];
    };
}

#endif