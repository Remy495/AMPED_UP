
#ifndef _SERCOM_INTERFACE_C_
#define _SERCOM_INTERFACE_C_

#include "sam.h"
#include "stdint.h"

#define SERCOM_DECLARATION(sercomNumber) \
    void sercom##sercomNumber##_enable(); \
    void sercom##sercomNumber##_disable(); \
    void sercom##sercomNumber##_setInterruptHandler(void(*isr)()); \
    void sercom##sercomNumber##_enableInterrupts(); \
    void sercom##sercomNumber##_disableInterrupts(); \
    Sercom* sercom##sercomNumber##_getRegistersPtr(); \
    void sercom##sercomNumber##_enablePin(uint32_t pinNumber); \
    void sercom##sercomNumber##_enableAltPin(uint32_t pinNumber)


#ifdef __cplusplus
extern "C"
{
#endif

SERCOM_DECLARATION(0);
SERCOM_DECLARATION(1);
SERCOM_DECLARATION(2);
SERCOM_DECLARATION(3);

#ifdef __cplusplus
}
#endif

#endif