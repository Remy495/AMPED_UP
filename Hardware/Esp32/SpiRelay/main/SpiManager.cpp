
#include <algorithm>
#include <iostream>

#include "driver/spi_slave.h"

#include "SpiManager.hpp"
#include "BluetoothManager.hpp"
#include "RemoteMessageQueue.hpp"
#include "BinaryUtil.hxx"

namespace
{

    DMA_ATTR AmpedUp::RemoteMessageQueue<10> outgoingSpiMessages{};

    void transactionReadyCallback(spi_slave_transaction_t*)
    {
        AmpedUp::SpiManager::setHandshakePin(1);
    }

    void transactionCompleteCallback(spi_slave_transaction_t*)
    {
        AmpedUp::SpiManager::setHandshakePin(0);
    }
}

AmpedUp::RemoteMessage& AmpedUp::SpiManager::beginOutgoingMessage()
{
    return outgoingSpiMessages.stageEnqueue();
}

AmpedUp::RemoteMessage& AmpedUp::SpiManager::getOutgoingMessage()
{
    return outgoingSpiMessages.getStagedMessage();
}

void AmpedUp::SpiManager::finishOutgoingMessage()
{
    outgoingSpiMessages.commitStagedEnqueue();
}

void AmpedUp::SpiManager::cancelOutgoingMessage()
{
    outgoingSpiMessages.cancelStagedEnqueue();
}

bool AmpedUp::SpiManager::isInitialized()
{
    return isInitialized_;
}

bool AmpedUp::SpiManager::run(gpio_num_t mosiPin, gpio_num_t misoPin, gpio_num_t sckPin, gpio_num_t ssPin, gpio_num_t handshakePin)
{

    bool status = true;

    // Initialize the outgoing message queue
    if (!outgoingSpiMessages.initialize())
    {
        printf("Failed to initialize outgoing SPI message queue\n");
        status = false;
    }

    // Configure the SPI peripheral
    if (status)
    {
        spi_bus_config_t spiBusConfig={
            .mosi_io_num=mosiPin,
            .miso_io_num=misoPin,
            .sclk_io_num=sckPin,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
        };

        spi_slave_interface_config_t spiSlaveConfig={
            .spics_io_num=ssPin,
            .flags=0,
            .queue_size=3,
            .mode=0,
            .post_setup_cb=transactionReadyCallback,
            .post_trans_cb=transactionCompleteCallback
        };

        // Enable pullup resistors to reduce noise on startup
        gpio_set_pull_mode(mosiPin, GPIO_PULLUP_ONLY);
        gpio_set_pull_mode(misoPin, GPIO_PULLUP_ONLY);
        gpio_set_pull_mode(sckPin, GPIO_PULLUP_ONLY);

        if (spi_slave_initialize(HSPI_HOST, &spiBusConfig, &spiSlaveConfig, 2) != ESP_OK)
        {
            printf("Failed to initialize SPI peripheral\n");
            status = false;
        }
    }

    // Configure the handshake pin
    if (status)
    {
        handshakePin_ = handshakePin;
        gpio_config_t handshakePinConfig = {
            .pin_bit_mask=(1ULL<<handshakePin),
            .mode=GPIO_MODE_OUTPUT,
            .intr_type=GPIO_INTR_DISABLE
        };
        gpio_config(&handshakePinConfig);
    }


    if (status)
    {
        isInitialized_ = true;
    }


    // Wait until Bluetooth manager is initialized before starting main loop
    while (!BluetoothManager::isInitialized());

    bool outgoingMessageSendHeader = true;
    bool incomingMessageReceiveHeader = true;

    while (true)
    {
        // Exit the main loop if a serious error has occured
        if (!status)
        {
            break;
        }

        // If there is nothing to send, send a "nothing to report" message
        RemoteMessage& fillerMessage = outgoingSpiMessages.stageEnqueue();
        if (outgoingSpiMessages.isEmpty())
        {
            fillerMessage.setHeader(RemoteMessageHeader(RemoteMessageType::NOTHING_TO_REPORT));
            outgoingSpiMessages.commitStagedEnqueue();
        }
        else
        {
            outgoingSpiMessages.cancelStagedEnqueue();
        }


        // Get the size of the outgoing message and a pointer to the next data to transmit (either the header or the payload)
        RemoteMessage& outgoingMessage = outgoingSpiMessages.peekFront();
        auto [outgoingMessageSize, outgoingMessageDataPtr] = getTransmissionAttributes(outgoingMessage, outgoingMessageSendHeader);


        // Get the size of the incoming message and a pointer to the next data to received. (either the header or the payload)
        // Using stage enqueue because the resulting message is not ready to be dequeued.
        // If we are receiving the header than start a new message, if we are receiving the payload than continue with the message in progress
        RemoteMessage& incomingMessage = incomingMessageReceiveHeader ? BluetoothManager::beginOutgoingMessage() : BluetoothManager::getOutgoingMessage();
        auto [incomingMessageSize, incomingMessageDataPtr] = getTransmissionAttributes(incomingMessage, incomingMessageReceiveHeader);

        uint32_t transactionSize = std::max(outgoingMessageSize, incomingMessageSize);

        // Prepare an SPI transaction to send the data to be sent and receive the data to be received
        spi_slave_transaction_t currentTransaction{};
        // Note that the transaction size is specified in bits and must be a while number of words
        currentTransaction.length = BinaryUtil::bitsFillWords(BinaryUtil::bytesToBits(transactionSize));
        currentTransaction.tx_buffer = outgoingMessageDataPtr;
        currentTransaction.rx_buffer = incomingMessageDataPtr;


        // Execute the SPI transaction
        spi_slave_transmit(HSPI_HOST, &currentTransaction, portMAX_DELAY);


        // Transaction complete. Finalize it and prepare the next transaction

        // If a message header was just sent and the message has a payload, send the payload next. Otherwise send the next message's header.
        if (outgoingMessageSendHeader && outgoingMessage.getHeader().hasPayload())
        {
            outgoingMessageSendHeader = false;
        }
        else
        {
            outgoingSpiMessages.dequeue();
            outgoingMessageSendHeader = true;

            // Alert the bluetooth manager that a message it received has finished being processed
            BluetoothManager::messageProcessingFinished();
        }

        // If we just received the payload, pass it along to the bluetooth manager. If we just received the header and
        // it is valid an has a payload, receive the payload. Otherwise discard it and receive the next message header
        if (incomingMessageReceiveHeader)
        {
            if (incomingMessage.headerIsValid() && incomingMessage.getHeader().hasPayload())
            {
                incomingMessageReceiveHeader = false;
            }
            else
            {
                BluetoothManager::cancelOutgoingMessage();
                incomingMessageReceiveHeader = true;
            }
            
        }
        else
        {
            BluetoothManager::finishOutgoingMessage();
            incomingMessageReceiveHeader = true;
        }
    }

    return status;
}

void AmpedUp::SpiManager::setHandshakePin(uint32_t level)
{
    gpio_set_level(handshakePin_, level);
}

AmpedUp::SpiManager::transmissionAttributes_t AmpedUp::SpiManager::getTransmissionAttributes(RemoteMessage& spiMessage, bool sendHeader)
{
    transmissionAttributes_t retVal;
    if (sendHeader)
    {
        retVal.size_ = sizeof(RemoteMessageHeader);
        retVal.dataPtr_ = spiMessage.getHeaderData();
    }
    else
    {
        retVal.size_ = spiMessage.getHeader().getFragmentPayloadSize();
        retVal.dataPtr_ = spiMessage.getPayloadData();
    }
    return retVal;
}