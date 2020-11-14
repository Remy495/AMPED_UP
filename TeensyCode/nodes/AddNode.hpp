

#ifndef _ADD_NODE_
#define _ADD_NODE_

#include "IntermediateNode.hpp"

namespace AmpedUp {

    class AddNode: public IntermediateNode<2, 1> {
    public:

        AddNode() {
            nodeType_ = AmpedUpNodes::NodeType::ADD;
        }

        void update() {
            outputs_[0] = inputs_[0].getValue() + inputs_[1].getValue();
        }
    };
}

#endif