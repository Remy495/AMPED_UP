

#ifndef _NODE_GRAPH_CONTEXT_
#define _NODE_GRAPH_CONTEXT_

#include "Time.hpp"

namespace AmpedUp
{
    struct NodeGraphContext
    {
        float guitarPitch;
        float guitarVolume;
        float expressionPedalPosition;
        Time currentTime;
        Time cycleElapsedTime;
    };
}

#endif