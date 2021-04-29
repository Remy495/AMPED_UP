

#ifndef _SINE_WAVE_NODE_
#define _SINE_WAVE_NODE_


#include "IntermediateNode.hpp"
#include "arm_math.h"

namespace AmpedUp
{
    class SineWaveNode: public IntermediateNode<3, 1> {
    public:

        SineWaveNode() {
            nodeType_ = AmpedUpNodes::NodeType::SINE_WAVE;
        }

        void update() {
            
            float phaseShiftRadians = inputs_[2].getValue() * 3.1415926535 / 180;
            float angularFrequency = inputs_[0].getValue() * 6.283;

            float elapsedTimeSeconds = ownerPtr_->getContext().cycleElapsedTime.getSeconds();
            
            currentArg_ += elapsedTimeSeconds * angularFrequency;

            while(currentArg_ > 6.2831853)
            {
                currentArg_ -= 6.2831853;
            }


            outputs_[0] = 0.5 + inputs_[1].getValue() * arm_sin_f32(currentArg_ + phaseShiftRadians) / 2;

        }

    private:
        float currentArg_{};
    };
}

#endif