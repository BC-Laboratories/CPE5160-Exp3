#ifndef _STA013_H
#define _STA013_H

#include "I2C.h"

#define timeout_val (1/(2*I2C_freq))
#define no_errors 0x00

uint8_t STA013_init(void);

#endif
