

#ifndef _MULTIPLY_NODE_
#define _MULTIPLY_NODE_

#include "IntermediateNode.hpp"

namespace AmpedUp {

    class MultiplyNode: public IntermediateNode<2, 1> {
    public:

        MultiplyNode() {
            nodeType_ = AmpedUpNodes::NodeType::MULTIPLY;
        }

        void update() {
            outputs_[0] = inputs_[0].getValue() * inputs_[1].getValue();
        }
    };
}

#endif