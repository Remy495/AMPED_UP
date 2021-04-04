


#include "I2cMessager.h"
#include "KnobMessage.hxx"
#include "ConstantsCommon.hpp"

	
int main(void)
{
	auto& messager = I2cMessager<SercomHandle::SERCOM_3, AmpedUp::KnobMessage>::getInstance();
	messager.begin(AmpedUp::Constants::DAUGHTER_BOARD_BASE_ADDRESS, false);

	while(1)
	{
		if (messager.hasReceivedMessage())
		{
			AmpedUp::KnobMessage receivedMessage = messager.takeReceivedMessage();
			float receivedKnobPosition = receivedMessage.getValue();

			AmpedUp::KnobMessage outgoingMessage(AmpedUp::KnobMessageType::CURRENT_KNOB_POSITION, receivedKnobPosition);
			messager.setReply(receivedMessage);
		}

	}
	
	return 0;
}