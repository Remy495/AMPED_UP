

#ifndef _GUITAR_SIGNAL_NODE_
#define _GUITAR_SIGNAL_NODE_

#include <algorithm>

#include "InputNode.hpp"
#include "IntermediateNode.hpp"
#include "NodeGraph.hpp"

namespace AmpedUp {

    class GuitarSignalNode: public IntermediateNode<1, 2> {
    public:

        GuitarSignalNode() {
            nodeType_ = AmpedUpNodes::NodeType::GUITAR_SIGNAL;
        }

        void update() {
            outputs_[0] = ownerPtr_->getContext().guitarVolume;
            outputs_[1] = ownerPtr_->getContext().guitarPitch;
        }
    };
}


#endif