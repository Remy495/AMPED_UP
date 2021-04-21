

#include "stdint.h"

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

void begin(uint8_t addressIndex);

void setAddress(uint8_t address);

float requestedPosition();

void actualPosition(float position, bool grabbed);

#ifdef __cplusplus
}
#endif