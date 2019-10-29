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
	uint8_t return_value;


	uint8_t i = timeout_val;
	uint8_t send_array[3] = {0x03,0x02,0x01};
	uint8_t array_name[3];

	do
	{
		I2C_write(0x43, 0x01, 1, 0, send_array);
		return_value = I2C_read(0x43,0,0,3,array_name);
		i--;
	}while((return_value!=no_errors)&&(i!=0));
	printf("Received Value = %2.2bX\n\r",array_name[0]);
	return return_value;

	//printf("Received Value = %2.2bX\n\r",array_name[1]);
	//printf("Received Value = %2.2bX\n\r",array_name[2]);
/*
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
    uint8_t internal_addr;
	uint8_t timeout;
	uint8_t status;
    uint16_t i = 0;
    
    do 
	{
        timeout = 50;
        
        internal_addr = config_array[i];
        ++i;
        
        send_array[0] = config_array[i];
        ++i;
        
        do 
		{
            status = I2C_write(device_addr, internal_addr, 1, 1, send_array);
            --timeout;
        } while ((status != no_errors) && (timeout != 0));
		I2C_read(device_addr, internal_addr,1,1,recv_array);
    } while ((internal_addr != 0xFF) && (timeout != 0));
    return status;
}
