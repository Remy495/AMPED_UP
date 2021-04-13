
#include <algorithm>
#include <iostream>

#include "driver/spi_slave.h"

#include "SpiManager.hpp"
#include "BluetoothManager.hpp"
#include "RemoteMessageHeader.hpp"
#include "BinaryUtil.hxx"
#include "TypedBuffer.h"


namespace
{

    DMA_ATTR uint8_t incomingMessagePayload[AmpedUp::Constants::REMOTE_MESSAGE_MTU];
    DMA_ATTR uint8_t outgoingMessagePayload[AmpedUp::Constants::REMOTE_MESSAGE_MTU];

    DMA_ATTR WordAlignedTypedBuffer<AmpedUp::RemoteMessageHeader> incomingMessageHeader;
    DMA_ATTR WordAlignedTypedBuffer<AmpedUp::RemoteMessageHeader> outgoingMessageHeader;

    uint32_t outgoingMessageBytesSent_{};

    void transactionReadyCallback(spi_slave_transaction_t*)
    {
        AmpedUp::SpiManager::setHandshakePin(1);
    }

    void transactionCompleteCallback(spi_slave_transaction_t*)
    {
        AmpedUp::SpiManager::setHandshakePin(0);
    }
}

bool AmpedUp::SpiManager::isInitialized()
{
    return isInitialized_;
}

bool AmpedUp::SpiManager::run(gpio_num_t mosiPin, gpio_num_t misoPin, gpio_num_t sckPin, gpio_num_t ssPin, gpio_num_t handshakePin)
{

    bool status = true;

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

    incomingMessageHeader = RemoteMessageHeader();
    outgoingMessageHeader = RemoteMessageHeader();

    // Initialization complete
    if (status)
    {
        isInitialized_ = true;
    }

    // Wait until Bluetooth manager is initialized before starting main loop
    while (!BluetoothManager::isInitialized());

    bool sendHeader = true;

    while (true)
    {
        // Exit the main loop if a serious error has occured
        if (!status)
        {
            break;
        }

        // Prepare an SPI transaction to send the data to be sent and receive the data to be received
        spi_slave_transaction_t currentTransaction{};

        if (sendHeader)
        {
            // Prepare outgoing message header
            
            RemoteMessageHeader& outgoingHeader = outgoingMessageHeader.getInstance();
            outgoingHeader.clearFlags();

            if (outgoingMessageBytesSent_ == 0)
            {
                // Begin the new message
                outgoingHeader.setTotalPayloadSize(BluetoothManager::getIncomingMessageSize());
                if (outgoingHeader.getTotalPayloadSize() > 0)
                {
                    outgoingHeader.setFlag(RemoteMessageFlag::FIRST_FRAGMENT);
                }
            }

            // Determine the appropriate size for this fragment
            RemoteMessageSize_t fragmentSize = BluetoothManager::getIncomingMessageAvailableSize();
            fragmentSize = std::min(fragmentSize, Constants::REMOTE_MESSAGE_MTU);
            fragmentSize = std::min(fragmentSize, outgoingHeader.getTotalPayloadSize());

            outgoingHeader.setFragmentPayloadSize(fragmentSize);

            // Set the "last fragment" flag if this message will finish sending the payload
            if (outgoingHeader.hasPayload() && outgoingMessageBytesSent_ + fragmentSize >= outgoingHeader.getTotalPayloadSize())
            {
                outgoingHeader.setFlag(RemoteMessageFlag::LAST_FRAGMENT);
            }

            // Set the "ready to recieve" flag if the Bluetooth manager is ready to send a message
            if (BluetoothManager::readyToSend())
            {
                outgoingHeader.setFlag(RemoteMessageFlag::READY_TO_RECIEVE);
            }

            // Set the "remote connected" flag if this is the first message since a new remote connected
            if (newConnectionWasMade_)
            {
                outgoingHeader.setFlag(RemoteMessageFlag::REMOTE_CONNECTED);
                newConnectionWasMade_ = false;
            }

            // Set the "remote disconnected" flag if this is the first message since the previously connected remote disconnected
            if (connectionWasJustLost_)
            {
                outgoingHeader.setFlag(RemoteMessageFlag::REMOTE_DISCONNECTED);
                connectionWasJustLost_ = false;
            }

            std::cout << "outgoing header: " << outgoingHeader << std::endl;

            // Make sure the header that already exists in the incoming buffer is invalidated to ensure we don't mistake an interrupted transfer as successful
            RemoteMessageHeader& incomingHeader = incomingMessageHeader.getInstance();
            incomingHeader.invalidate();

            // Set the SPI transaction to send / receive the message headers
            currentTransaction.length = BinaryUtil::bytesToBits(incomingMessageHeader.getSize());
            currentTransaction.tx_buffer = outgoingMessageHeader.getData();
            currentTransaction.rx_buffer = incomingMessageHeader.getData();
        }
        else
        {

            // Prepare outgoing data and determine the size of the SPI transaction needed to transfer incoming / outoging data
            RemoteMessageHeader& outgoingHeader = outgoingMessageHeader.getInstance();
            RemoteMessageHeader& incomingHeader = incomingMessageHeader.getInstance();

            RemoteMessageSize_t transactionSize = 0;

            if (outgoingHeader.hasPayload() && incomingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
            {
                transactionSize = outgoingHeader.getFragmentPayloadSize();
                BluetoothManager::recieveIncomingMessage(outgoingMessagePayload, outgoingHeader.getFragmentPayloadSize());
                outgoingMessageBytesSent_ += outgoingHeader.getFragmentPayloadSize();

                if (outgoingMessageBytesSent_ >= outgoingHeader.getTotalPayloadSize())
                {
                    // If this transfer completes the message, the next payload should be the beginning of the next message
                    outgoingMessageBytesSent_ = 0;
                }
            }

            if (incomingHeader.hasPayload() && outgoingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
            {
                transactionSize = std::max(transactionSize, incomingHeader.getFragmentPayloadSize());
            }

            // Set the SPI transaction to send / recieve the message payloads
            currentTransaction.length = BinaryUtil::bitsFillWords(BinaryUtil::bytesToBits(transactionSize));
            currentTransaction.tx_buffer = outgoingMessagePayload;
            currentTransaction.rx_buffer = incomingMessagePayload;
        }

        // Execute the SPI transaction
        spi_slave_transmit(HSPI_HOST, &currentTransaction, portMAX_DELAY);

        if (sendHeader)
        {

            // Determine whether the next transaction should be a header or a payload based on the headers that were transfered
            RemoteMessageHeader& outgoingHeader = outgoingMessageHeader.getInstance();
            RemoteMessageHeader& incomingHeader = incomingMessageHeader.getInstance();

            std::cout << "incoming header: " << incomingHeader << std::endl;

            // Payload should be transfered if at least one side has a payload to send and at least the other side is ready to recieve it
            // If the recieved header is not valid then there probably wasn't a real transaction, just noise on the line, so ignore it and transfer another header
            if (((outgoingHeader.hasPayload() && incomingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE)) ||
                 (incomingHeader.hasPayload() && outgoingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))) &&
                 incomingHeader.isValid())
            {
                sendHeader = false;
            }

        }
        else
        {
            // Process the data that was just recieved and pass it along to the bluetooth manager
            RemoteMessageHeader& outgoingHeader = outgoingMessageHeader.getInstance();
            RemoteMessageHeader& incomingHeader = incomingMessageHeader.getInstance();

            if (incomingHeader.hasPayload() && outgoingHeader.hasFlag(RemoteMessageFlag::READY_TO_RECIEVE))
            {
                std::cout << "Recieved payload (" << incomingHeader.getTotalPayloadSize() << " byte message, " << incomingHeader.getFragmentPayloadSize() << " byte fragment)" << std::endl;

                if (incomingHeader.hasFlag(RemoteMessageFlag::FIRST_FRAGMENT))
                {
                    BluetoothManager::sendNewMessage(incomingMessagePayload, incomingHeader.getTotalPayloadSize(), incomingHeader.getFragmentPayloadSize());
                }
                else
                {
                    BluetoothManager::sendContinuingMessage(incomingMessagePayload, incomingHeader.getFragmentPayloadSize());
                }
            }

            // Always transfer header after transfering payload
            sendHeader = true;
        }
        
    }

    return status;
}

void AmpedUp::SpiManager::notifyNewConnection()
{
    newConnectionWasMade_ = true;
}

void AmpedUp::SpiManager::notifyNewDisconnection()
{
    connectionWasJustLost_ = true;
}

void AmpedUp::SpiManager::setHandshakePin(uint32_t level)
{
    gpio_set_level(handshakePin_, level);
}
