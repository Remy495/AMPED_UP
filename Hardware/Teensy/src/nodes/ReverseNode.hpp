

#ifndef _REVERSE_NODE_
#define _REVERSE_NODE_

#include "IntermediateNode.hpp"
#include "Arduino.h"

namespace AmpedUp {

    class ReverseNode: public IntermediateNode<1, 1> {
    public:

        ReverseNode() {
            nodeType_ = AmpedUpNodes::NodeType::REVERSE;
        }

        void update() {
            outputs_[0] = 1 - inputs_[0].getValue();
        }
    };
}

#endif