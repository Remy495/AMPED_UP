
#include "TextLogging.hxx"
#include "Arduino.h"
#include "RemoteCommunication.hpp"

#include "SpiMessager.hpp"


void AmpedUp::RemoteCommunication::initialize(uint32_t highWaterMark)
{
    SpiMessager::begin();
    highWaterMark_ = highWaterMark;
}

bool AmpedUp::RemoteCommunication::isReadyToSend(bool isImportant)
{
    return !outgoingMessageInProgress_ && SpiMessager::isReadyToSend(Constants::REMOTE_MESSAGE_MAX_SIZE) && (SpiMessager::outgoingMessageCount() < highWaterMark_ || isImportant);
}

flatbuffers::FlatBufferBuilder& AmpedUp::RemoteCommunication::beginOutgoingMessage()
{
    currentMessageBuilderContainer_.emplace(1024, &allocator_);
    return currentMessageBuilderContainer_.getInstance();
}

void AmpedUp::RemoteCommunication::sendFinishedMessage()
{
    uint32_t outgoingMessageSize = currentMessageBuilderContainer_.getInstance().GetSize();

    SpiPayload outgoingPayload = SpiMessager::getStagedOutgoingMessage();
    if (outgoingPayload.getSize() > outgoingMessageSize)
    {
        outgoingPayload = SpiMessager::trimStagedOutgoingMessage(currentMessageBuilderContainer_.getInstance().GetBufferPointer(), outgoingMessageSize);
    }

    SpiMessager::sendStagedMessage();
    currentMessageBuilderContainer_.deleteInstance();
}

bool AmpedUp::RemoteCommunication::hasRecievedMessage()
{
    bool hasRecievedValidMessage = false;

    // Check for valid messages in the recieve queue until one is found or the queue is empty
    while (!hasRecievedValidMessage && SpiMessager::hasRecievedMessage())
    {
        if (verifyMessage(SpiMessager::peekRecievedMessage()))
        {
            hasRecievedValidMessage = true;
        }
        else
        {
            TextLogging::critical(__FILE__, __LINE__, "Recieved invalid remote message");

            // Discard the invalid message to make room for a valid message
            SpiMessager::discardRecievedMessage();
        }
    }

    return hasRecievedValidMessage;
}

const AmpedUpMessaging::Message* AmpedUp::RemoteCommunication::getRecievedMessage()
{
    SpiPayload recievedMessagePayload = SpiMessager::peekRecievedMessage();
    return AmpedUpMessaging::GetMessage(recievedMessagePayload.getData());
}

void AmpedUp::RemoteCommunication::discardRecievedMessage()
{
    SpiMessager::discardRecievedMessage();
}

bool AmpedUp::RemoteCommunication::remoteIsConnected()
{
    if (SpiMessager::remoteHasConnected())
    {
        remoteIsConnected_ = true;
    }

    if (SpiMessager::remoteHasDisconnected())
    {
        remoteIsConnected_ = false;
    }

    return remoteIsConnected_;
}

bool AmpedUp::RemoteCommunication::verifyMessage(const SpiPayload& messagePayload)
{
    flatbuffers::Verifier bufferVerifier(messagePayload.getData(), messagePayload.getSize());
    return AmpedUpMessaging::VerifyMessageBuffer(bufferVerifier);
}