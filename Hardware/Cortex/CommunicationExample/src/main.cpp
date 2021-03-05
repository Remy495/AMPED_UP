


#include "I2cMessager.h"

uint32_t messageValue{};
	
int main(void)
{
	auto& messager = I2cMessager<SercomHandle::SERCOM_3, uint32_t>::getInstance();
	messager.begin(8, false);
	messager.setReply(messageValue);

	while(1)
	{
		if (messager.hasReceivedMessage())
		{
			uint32_t receivedMessage = messager.takeReceivedMessage();
			messager.setReply(receivedMessage);
		}

	}
	
	return 0;
}