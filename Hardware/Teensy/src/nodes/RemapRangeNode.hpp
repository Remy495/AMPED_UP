

#ifndef _REMAP_RANGE_
#define _REMAP_RANGE_

#include "IntermediateNode.hpp"

namespace AmpedUp {

    class RemapRangeNode: public IntermediateNode<3, 1> {
    public:

        RemapRangeNode() {
            nodeType_ = AmpedUpNodes::NodeType::REMAP_RANGE;
        }

        void update() {
            float outputMin = std::min(inputs_[1].getValue(), inputs_[2].getValue());
            float outputMax = std::max(inputs_[1].getValue(), inputs_[2].getValue());

            float outputValue = (inputs_[0].getValue()) * (inputs_[2].getValue() - inputs_[1].getValue()) + inputs_[1].getValue();
            outputs_[0] = std::max(std::min(outputValue, outputMax), outputMin);
        }
    };
}

#endif
