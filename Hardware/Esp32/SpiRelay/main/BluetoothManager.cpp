
#include "esp_attr.h"
#include "btstack_port_esp32.h"
#include "btstack_run_loop.h"
#include "hci_dump.h"
#include "btstack.h"

#include "ConstantsCommon.hpp"
#include "BluetoothManager.hpp"
#include "SpiManager.hpp"
#include "RemoteMessageQueue.hpp"

namespace
{
    // Outgoing queue must be DMA-accessable so it can be accessed by SPI manager
    DMA_ATTR AmpedUp::RemoteMessageQueue<10> outgoingBluetoothMessages{};

    btstack_packet_callback_registration_t hci_event_callback_registration;
    uint8_t  spp_service_buffer[150];
}

AmpedUp::RemoteMessage& AmpedUp::BluetoothManager::beginOutgoingMessage()
{
    return outgoingBluetoothMessages.stageEnqueue();
}

AmpedUp::RemoteMessage& AmpedUp::BluetoothManager::getOutgoingMessage()
{
    return outgoingBluetoothMessages.getStagedMessage();
}

void AmpedUp::BluetoothManager::finishOutgoingMessage()
{
    outgoingBluetoothMessages.commitStagedEnqueue();
}

void AmpedUp::BluetoothManager::cancelOutgoingMessage()
{
    outgoingBluetoothMessages.cancelStagedEnqueue();
}

void AmpedUp::BluetoothManager::messageProcessingFinished()
{

}

bool AmpedUp::BluetoothManager::isInitialized()
{
    return isInitialized_;
}

bool AmpedUp::BluetoothManager::run()
{
    // Initialize Bluetooth manager

    bool status = true;
    if (!outgoingBluetoothMessages.initialize())
    {
        printf("Failed to initialize outgoing bluetooth message queue\n");
        status = false;
    }

    // Initialize BTstack
    btstack_init();

    // SPP service, packet handler, make discoverable

    hci_event_callback_registration.callback = BluetoothManager::packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    rfcomm_init();
    rfcomm_register_service(BluetoothManager::packet_handler, 1, Constants::REMOTE_MESSAGE_MTU);

    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, 0x10001, 1, "Amped Up Remote Control");
    sdp_register_service(spp_service_buffer);
    sdp_init();

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

void AmpedUp::BluetoothManager::packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size)
{
    if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_INCOMING_CONNECTION)
    {
        // Accept all connections
        uint16_t rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
        rfcomm_accept_connection(rfcomm_channel_id);
    }
    else if (packet_type == RFCOMM_DATA_PACKET)
    {
        printf("Received data: '");
        for (uint16_t i = 0; i < size; i++)
        {
            putchar(packet[i]);
        }
        printf("'\n");

        AmpedUp::RemoteMessage& message = AmpedUp::SpiManager::beginOutgoingMessage();
        message.setHeader(AmpedUp::RemoteMessageHeader(AmpedUp::RemoteMessageType::REMOTE_MESSAGE, 12));
        message.setPayloadData(packet, size);
        AmpedUp::SpiManager::finishOutgoingMessage();
    }
}