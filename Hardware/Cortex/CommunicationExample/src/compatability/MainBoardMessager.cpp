
#include "MainBoardMessager.h"

#include "I2cMessager.h"
#include "KnobMessage.hxx"
#include "ConstantsCommon.hpp"

using MainBoardMessager = I2cMessager<SercomHandle::SERCOM_3, AmpedUp::KnobMessage>;
float lastRequestedPosition = 0;

void begin(uint8_t addressIndex)
{
    MainBoardMessager::getInstance().begin(addressIndex + AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);
}

void setAddress(uint8_t address)
{
    MainBoardMessager::getInstance().setAddress(address + AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS);
}

float requestedPosition()
{
    auto& mbm = MainBoardMessager::getInstance();
    if (mbm.hasReceivedMessage())
    {
        lastRequestedPosition = mbm.takeReceivedMessage().getValue();
    }

    return lastRequestedPosition;
}

void actualPosition(float position, bool grabbed)
{
    AmpedUp::KnobMessage outgoingMessage;
    if (grabbed)
    {
        outgoingMessage.setMessageType(AmpedUp::KnobMessageType::SET_KNOB_POSITION);
    }
    else
    {
        outgoingMessage.setMessageType(AmpedUp::KnobMessageType::CURRENT_KNOB_POSITION);
    }

    outgoingMessage.setValue(position);

    MainBoardMessager::getInstance().setReply(outgoingMessage);
}