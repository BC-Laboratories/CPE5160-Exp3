/* Names: Tyler Andrews, Tyler Tetens, Brennan Campbell
 * Experiment 3
 */

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

/* Desc:
 *
 */
uint8_t STA013_config(uint8_t device_addr, uint8_t * config_array);

#endif
