#ifndef _STA013_H
#define _STA013_H

#include "I2C.h"

// value used for timeouts during init
#define timeout_val (1/(2*I2C_freq))
// error values
#define no_errors 0x00

/* Desc:
 *
 */
uint8_t STA013_init(void);

#endif
