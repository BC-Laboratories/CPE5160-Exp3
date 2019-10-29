/* Names: Tyler Andrews, Tyler Tetens, Brennan Campbell
 * Experiment 3
 */

#include "Main.h"
#include "STA013.h"
#include "I2C.h"
#include <stdio.h>
#include "Timer0_hardware_delay_1ms.h"

extern uint8_t code CONFIG;
extern uint8_t code CONFIG2;

uint8_t STA013_init(void)
{
	uint8_t return_value; // variable used for error status
	uint8_t index = timeout_val;
	uint8_t send_array[3];
	uint8_t recv_array[3];

	do
	{
		I2C_write(STA013_device_addr, 0x01, 1, 0, send_array); // setting internal address to 0x01
		return_value = I2C_read(STA013_device_addr,0,0,1,recv_array);
		index--;
	}while((return_value!=no_errors)&&(index!=0));
	printf("Received Value = %2.2bX\n\r",recv_array[0]);
	return return_value;

/* // Read and Send of each config file
	printf("--------------------Config 1 Go!!!!--------------------\r\n");
	STA013_config(0x43, &CONFIG);
    Timer0_DELAY_1ms(1000);
	printf("--------------------Config 2 Go!!!!--------------------\r\n");
    STA013_config(0x43, &CONFIG2);
    Timer0_DELAY_1ms(1000);  
	printf("Finished Init...\r\n");
	return return_value;
	*/
}


uint8_t STA013_config(uint8_t device_addr, uint8_t * config_array)
{
    uint8_t send_array[1];
	uint8_t recv_array[1];
    uint8_t internal_addr; // address fro config file
	uint8_t timeout;
	uint8_t status; // variable to hold error
    uint16_t index = 0;
    
    do 
	{
        timeout = 50; // simple timeout
        internal_addr = config_array[index];
        ++index;
        send_array[0] = config_array[index]; // data to set to address from config file
        ++index;
        do
		{
            status = I2C_write(device_addr, internal_addr, 1, 1, send_array);
            --timeout;
        } while ((status != no_errors) && (timeout != 0));
		I2C_read(device_addr, internal_addr,1,1,recv_array);
    } while ((internal_addr != 0xFF) && (timeout != 0)); // 0xFF is the end of the config file
    return status;
}
