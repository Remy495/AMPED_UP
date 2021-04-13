
#include "btstack.h"
 
static btstack_packet_callback_registration_t hci_event_callback_registration;
static uint8_t  spp_service_buffer[150];

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size);

int btstack_main(int argc, const char * argv[])
{
    hci_event_callback_registration.callback = packet_handler;
    hci_add_event_handler(&hci_event_callback_registration);

    l2cap_init();

    rfcomm_init();
    rfcomm_register_service(packet_handler, 1, 0xffff);

    memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
    spp_create_sdp_record(spp_service_buffer, 0x10001, 1, "Amped Up Remote Control");
    sdp_register_service(spp_service_buffer);
    sdp_init();

    gap_set_local_name("Amped Up!");
    gap_discoverable_control(1);

    hci_power_control(HCI_POWER_ON);

    return 0;
}

static void packet_handler(uint8_t packet_type, uint16_t channel, uint8_t* packet, uint16_t size)
{
    if (packet_type == HCI_EVENT_PACKET && hci_event_packet_get_type(packet) == RFCOMM_EVENT_INCOMING_CONNECTION)
    {
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
    }
}