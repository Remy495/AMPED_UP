
#include <algorithm>
#include <iostream>

#include "esp_attr.h"
#include "btstack_port_esp32.h"
#include "btstack_run_loop_freertos.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"
#include "btstack.h"

#include "ConstantsCommon.hpp"
#include "BluetoothManager.hpp"
#include "SpiManager.hpp"

namespace
{
    btstack_packet_callback_registration_t hci_event_callback_registration;
    uint8_t  spp_service_buffer[150];
}

bool AmpedUp::BluetoothManager::readyToSend()
{
    return outgoingDataQueue_.canWriteNewMessageData(Constants::REMOTE_MESSAGE_MTU);
}

void AmpedUp::BluetoothManager::sendNewMessage(const BinaryUtil::byte_t* data, RemoteMessageSize_t totalMessageSize, RemoteMessageSize_t fragmentMessageSize)
{
    outgoingDataQueue_.writeNewMessageData(data, totalMessageSize, fragmentMessageSize);
    sendIfNeededExternal();
}

void AmpedUp::BluetoothManager::sendContinuingMessage(const BinaryUtil::byte_t* data, RemoteMessageSize_t fragmentMessageSize)
{
    outgoingDataQueue_.writeContinuingMessageData(data, fragmentMessageSize);
    sendIfNeededExternal();
}

AmpedUp::RemoteMessageSize_t AmpedUp::BluetoothManager::getIncomingMessageSize()
{
    return incomingDataQueue_.getFrontMessageSize();
}

AmpedUp::RemoteMessageSize_t AmpedUp::BluetoothManager::getIncomingMessageAvailableSize()
{
    return incomingDataQueue_.getFrontMessageAvailableSize();
}

void AmpedUp::BluetoothManager::recieveIncomingMessage(BinaryUtil::byte_t* data, RemoteMessageSize_t fragmentMessageSize)
{
    incomingDataQueue_.readMessageData(data, fragmentMessageSize);
    grantCreditsExternal();
}

bool AmpedUp::BluetoothManager::isConnected()
{
    return isConnected_;
}

bool AmpedUp::BluetoothManager::isInitialized()
{
    return isInitialized_;
}

bool AmpedUp::BluetoothManager::run()
{
    // Initialize Bluetooth manager

    bool status = true;

    if (!outgoingDataQueue_.initialize())
    {
        std::cout << "Bluetooth manager failed to initialize outgoing data queue" << std::endl;
        status = false;
    }

    if (status && !incomingDataQueue_.initialize())
    {
        std::cout << "Bluetooth manager failed to initialize incoming data queue" << std::endl;
        status = false;
    }

    // Initialize BTstack
    btstack_init();

    // setup SPP service, packet handler

    hci_event_callback_registration.callback = BluetoothManager::packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    rfcomm_init();
    rfcomm_register_service_with_initial_credits(BluetoothManager::packet_handler, 1, Constants::REMOTE_MESSAGE_MTU, Constants::MINIMUM_QUEUEABLE_MESSAGES);

    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, 0x10001, 1, "Amped Up Remote Control");
    sdp_register_service(spp_service_buffer);
    sdp_init();

    // make discoverable

    gap_set_local_name("Amped Up!");
    gap_discoverable_control(1);

    hci_power_control(HCI_POWER_ON);

    if (status)
    {
        isInitialized_ = true;
    }

    // Wait until SPI manager is initialized before starting main loop
    while (!SpiManager::isInitialized());

    // Enter BTstack main loop
    btstack_run_loop_execute();

    return status;
}

void AmpedUp::BluetoothManager::sendIfNeededExternal()
{
    btstack_run_loop_freertos_execute_code_on_main_thread(sendIfNeededInternal, nullptr);
}

void AmpedUp::BluetoothManager::sendIfNeededInternal(void*)
{
    if (isConnected_ && outgoingDataQueue_.hasData())
    {
        rfcomm_request_can_send_now_event(currentRfcommChannel_);
    }
}

void AmpedUp::BluetoothManager::grantCreditsExternal()
{
    btstack_run_loop_freertos_execute_code_on_main_thread(grantCreditsInternal, nullptr);
}

void AmpedUp::BluetoothManager::grantCreditsInternal(void*)
{
    while(isConnected_ && incomingDataQueue_.getFreeSize() > (incomingCreditCount_ + 1) * Constants::REMOTE_MESSAGE_MTU)
    {
        rfcomm_grant_credits(currentRfcommChannel_, 1);
        incomingCreditCount_++;
    }
}

void AmpedUp::BluetoothManager::packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size)
{
    if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_INCOMING_CONNECTION)
    {
        // Occurs when a new connection is requested. Connection should be accepted if there is not already a connection
        uint16_t newConnectionChannel = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
        if (isConnected_)
        {
            rfcomm_decline_connection(newConnectionChannel);
        }
        else
        {
            currentRfcommChannel_ = newConnectionChannel;
            rfcomm_accept_connection(newConnectionChannel);
        }
    }
    else if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_CHANNEL_OPENED)
    {
        // Occurs when a new channel is opened (or fails to open)
        if (rfcomm_event_channel_opened_get_status(packet) == 0)
        {
            // Opened successfully
            incomingCreditCount_ = Constants::MINIMUM_QUEUEABLE_MESSAGES;
            isConnected_ = true;
            SpiManager::notifyNewConnection();
        }
        else
        {
            // Error occured
            std::cout << "Bluetooth connection failed" << std::endl;
            isConnected_ = false;
        }
    }
    else if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_CHANNEL_CLOSED)
    {
        // Occurs when a channel closes
        isConnected_ = false;
        SpiManager::notifyNewDisconnection();
    }
    else if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_CAN_SEND_NOW)
    {
        // Occurs when data can be sent over the bluetooth connection
        if (outgoingDataQueue_.hasData())
        {
            rfcomm_reserve_packet_buffer();
            uint16_t sizeToSend = std::min(outgoingDataQueue_.getUsedSize(), static_cast<uint32_t>(rfcomm_get_max_frame_size(currentRfcommChannel_)));
            BinaryUtil::byte_t* outgoingPacketPtr = rfcomm_get_outgoing_buffer();
            outgoingDataQueue_.readRaw(outgoingPacketPtr, sizeToSend);
            rfcomm_send_prepared(currentRfcommChannel_, sizeToSend);

            std::cout << "Sending packet (" << sizeToSend << " bytes)" << std::endl;

            sendIfNeededInternal();
        }
    }
    else if (packet_type == RFCOMM_DATA_PACKET)
    {
        // Occurs when data is recieved over the bluetooth connection
        incomingCreditCount_--;
        incomingDataQueue_.writeRaw(packet, size);
        grantCreditsInternal();

        std::cout << "Recieved packet (" << size << " bytes)" << std::endl;
        std::cout << "Incoming queue used size: " << incomingDataQueue_.getUsedSize() << "bytes" << std::endl;
        std::cout << incomingDataQueue_ << std::endl;
        std::cout << "Incoming credits: " << static_cast<uint32_t>(incomingCreditCount_) << std::endl;
    }
}