

#ifndef _EXPRESSION_PEDAL_NODE_
#define _EXPRESSION_PEDAL_NODE_

#include <algorithm>

#include "IntermediateNode.hpp"
#include "NodeGraph.hpp"

namespace AmpedUp {

    class ExpressionPedalNode: public IntermediateNode<1, 2> {
    public:

        ExpressionPedalNode() {
            nodeType_ = AmpedUpNodes::NodeType::EFFECTS_PEDAL;
        }

        void update() {
            outputs_[0] = ownerPtr_->getContext().expressionPedalPosition;
        }
    };
}

#endif