/* Names: Tyler Andrews, Tyler Tetens, Brennan Campbell
 * Experiment 3
 */

#include "Main.h"
#include "STA013.h"
#include "I2C.h"
#include <stdio.h>

uint8_t STA013_init(void)
{

	uint8_t i = timeout_val;
	uint8_t send_array[3] = {0x03,0x02,0x01};
	uint8_t array_name[3];
	uint8_t return_value;

	do
	{
		//I2C_write(0x43, 0x01, 8, 3 , send_array);
		return_value = I2C_read(0x43,0,0,3,array_name);
		i--;
	}while((return_value!=no_errors)&&(i!=0));
	printf("Received Value = %2.2bX\n\r",array_name[0]);
	printf("Received Value = %2.2bX\n\r",array_name[1]);
	printf("Received Value = %2.2bX\n\r",array_name[2]);
	return return_value;

}
