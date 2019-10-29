#ifndef _I2C_H
#define _I2C_H

#include "Main.h"

//error codes
#define no_errors 0x00
#define bus_busy_error 0x01
#define ack_error 0x02

#define ACK 0x00
#define NACK 0x01

#define Continue 0x00
#define Stop 0x01 // not sure what this does

#define I2C_freq 25000UL

#define RELOAD (65536-((OSC_FREQ)/(2*I2C_freq*OSC_PER_INST)))
#define I2C_RELOAD_H (RELOAD/256)
#define I2C_RELOAD_L (RELOAD%256)

uint8_t Ack_check(void);

uint8_t I2C_read(uint8_t device_addr, uint32_t internal_addr, uint8_t int_size, uint8_t num_bytes, uint8_t * store_data_arr);

uint8_t I2C_write(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * send_data_arr);

void I2C_clock_delay(uint8_t control);

void I2C_clock_start(void);

#endif
