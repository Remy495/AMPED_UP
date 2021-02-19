

#ifndef _OUTPUT_NODE_
#define _OUTPUT_NODE_

#include "Node.hpp"

namespace AmpedUp {

    template<size_t nInputs>
    class OutputNode: public Node {
    public:

        ///////////////////////////////////////////////////////
        /// @brief Virtual destructor
        ///////////////////////////////////////////////////////
        virtual ~OutputNode() = default;

        ///////////////////////////////////////////////////////
        /// @brief Get the number of outputs this node has (0, as this is an output node)
        ///////////////////////////////////////////////////////
        size_t numOutputs() const {
            return 0;
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
        /// @brief Get this node's output array; Returns nullptr, as this node has no inputs
        ///////////////////////////////////////////////////////
        const NodeValue* outputs() const {
            return nullptr;
        }

        NodeInput inputs_[nInputs];
    };
}

#endif