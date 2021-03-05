
#include "SercomInterface_C.h"
#include "SercomInterface.h"

#define SERCOM_HANDLE(sercomNumber) SercomHandle::SERCOM_##sercomNumber

#define SERCOM_DEFINITION(sercomNumber) \
    void sercom##sercomNumber##_enable() \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::enable(); \
    } \
    void sercom##sercomNumber##_disable() \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::disable(); \
    } \
    void sercom##sercomNumber##_setInterruptHandler(void(*isr)()) \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::setInterruptHandler(isr); \
    } \
    void sercom##sercomNumber##_enableInterrupts() \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::enableInterrupts(); \
    } \
    void sercom##sercomNumber##_disableInterrupts() \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::disableInterrupts(); \
    } \
    Sercom* sercom##sercomNumber##_getRegistersPtr() \
    { \
        return &SercomInterface<SERCOM_HANDLE(sercomNumber)>::getRegisters(); \
    } \
    void sercom##sercomNumber##_enablePin(uint32_t pinNumber) \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::getPins().enablePins(pinNumber); \
    } \
    void sercom##sercomNumber##_enableAltPin(uint32_t pinNumber) \
    { \
        SercomInterface<SERCOM_HANDLE(sercomNumber)>::getAltPins().enablePins(pinNumber); \
    }

// TODO: fix swallowed semicolon

SERCOM_DEFINITION(0)
SERCOM_DEFINITION(1)
SERCOM_DEFINITION(2)
SERCOM_DEFINITION(3)