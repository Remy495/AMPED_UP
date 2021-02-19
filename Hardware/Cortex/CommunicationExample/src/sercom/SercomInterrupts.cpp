
#include "sam.h"

#include "SercomConfig.h"

// Interrupt handlers for sercom units. Simply calls the application-supplied interrupt handler for the sercom in question

void SERCOM0_Handler()
{
    SercomConfig<SercomHandle::SERCOM_0>::interruptHandler();
}

void SERCOM1_Handler()
{
    SercomConfig<SercomHandle::SERCOM_1>::interruptHandler();
}

void SERCOM2_Handler()
{
    SercomConfig<SercomHandle::SERCOM_2>::interruptHandler();
}

void SERCOM3_Handler()
{
    SercomConfig<SercomHandle::SERCOM_3>::interruptHandler();
}