

#ifndef _AVERAGE_NODE_
#define _AVERAGE_NODE_

#include "IntermediateNode.hpp"
#include "Arduino.h"

namespace AmpedUp {

    class AverageNode: public IntermediateNode<2, 1> {
    public:

        AverageNode() {
            nodeType_ = AmpedUpNodes::NodeType::AVERAGE;
        }

        void update() {
            outputs_[0] = (inputs_[0].getValue() + inputs_[1].getValue()) / 2;
        }
    };
}

#endif