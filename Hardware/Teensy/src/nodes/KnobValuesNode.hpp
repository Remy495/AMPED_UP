
#ifndef _KNOB_VALUES_NODE_
#define _KNOB_VALUES_NODE_

#include <algorithm>

#include "IntermediateNode.hpp"

namespace AmpedUp
{
    class KnobValuesNode : public IntermediateNode<8, 8>
    {

    public:

        KnobValuesNode()
        {
            nodeType_ = AmpedUpNodes::NodeType::KNOB_POSITIONS;
        }

        void update()
        {
            for (int i = 0; i < 8; i++)
            {
                float knobValue = inputs_[i].getValue() * (knobMaxes_[i] - knobMins_[i]) + knobMins_[i];
                knobValue = std::min(std::max(knobValue, knobMins_[i]), knobMaxes_[i]);

                knobValue = knobValue * 0.95 + 0.025;

                outputs_[i] = knobValue;
            }
        }

        void setKnobMin(int knobIndex, float minimum)
        {
            knobMins_[knobIndex] = minimum;
        }

        void setKnobMax(int knobIndex, float maximum)
        {
            knobMaxes_[knobIndex] = maximum;
        }

    private:
        float knobMins_[8]{0, 0, 0, 0, 0, 0, 0, 0};
        float knobMaxes_[8]{1, 1, 1, 1, 1, 1, 1, 1};
    };
}

#endif