/* Names: Tyler Andrews, Tyler Tetens, Brennan Campbell
 * Experiment 3
 */

#include "Main.h"
#include "I2C.h"
#include <stdio.h>

// Globals used in these functions
sbit SDA = P1^0;
sbit SCL = P1^1;

uint8_t Ack_Check(void)
{
    //variable to hold the ack/nack
	bit response_bit;

	I2C_clock_delay(Continue);
	SCL = 0; // Pull clock low to set new bit
	SDA = 1; // Set bit high
	I2C_clock_delay(Continue);
	SCL = 1;
	while(SCL != 1);
	response_bit = SDA;
	// Check for ack or nack
	if(response_bit != 1)
	{
		printf("Received Ack...\r\n");
		return ACK;
	}
	else
	{
		printf("Received Nack...\r\n");
		return NACK;
	}
}

uint8_t I2C_read(uint8_t device_addr, uint32_t internal_addr, uint8_t int_size, uint8_t num_bytes, uint8_t * store_data_arr)
{
	uint8_t send_val = 0; // byte to send
	uint8_t index = 0;
	uint8_t num_bits = 0;
	uint8_t recv_value = 0; // byte that is received
	bit sent_bit = 0;
	uint8_t return_value = no_errors; // return value with error code
	bit send_bit = 0;

	// Send an internal address if there is one
	if(int_size>0)
	{
		printf("Sending an internal address for a read");
		return_value = I2C_write(device_addr,internal_addr, int_size,0,0);
		printf("Done sending an internal address for a read");
	}

	// Default both SDA and SCL to 1
	SDA = 1;
	SCL = 1;

	if((SCL==1)&&(SDA==1))
	{
		I2C_clock_start();
		send_val = device_addr << 1; // 7 bit device address
		send_val |= 0x01; // Set R/W bit, 1 = Read

		// Start condition
		SDA = 0;
		index = 0;
		
		//send device addr
		printf("Sending device addr.\r\n");
		num_bits = 8;
		do
		{
			I2C_clock_delay(Continue);
			SCL = 0; // Pull the clock low before transitioning to new bit
			num_bits--;
			send_bit = ((send_val>>num_bits)&0x01); // Shift down and mask off the upper 7 bits
			SDA = send_bit;
			I2C_clock_delay(Continue);
			SCL=1; // Pull the clock back high to sample
			while(SCL!=1);
			// collision detection
			sent_bit = SDA;
			if(sent_bit!=send_bit)
			{
				return_value=bus_busy_error;
			}
		}while((num_bits!=0)&&(return_value==no_errors));
		// Wait for ACK/NACK
		if(Ack_check() != ACK)
		{
			return ack_error;
		}

		// Read num_bytes from address
		// Outer while loop is for each byte
		while((num_bytes>0)&&(return_value == no_errors))
		{
		    // Do-while loop is for each bit
			num_bits = 8;
			do
			{
				I2C_clock_delay(Continue);
				SCL=0; // Pull clock low and transition to new bit
				num_bits--;
				SDA = 1;
				recv_value = recv_value << 1;
				I2C_clock_delay(Continue);
				SCL=1; // Pull clock high to sample
				while(SCL!=1);
				sent_bit = SDA;
				recv_value |= sent_bit;
			}while(num_bits!=0);
			// Store the byte in the array buffer
			*(store_data_arr+index) = recv_value;
			index++;
			num_bytes--;
			// Set Ack/Nack if last byte
			if(num_bytes == 0)
			{
				send_bit=1;
			}
			else
			{
				send_bit=0;
			}
			I2C_clock_delay(Continue);
			SCL=0;
			SDA=send_bit;
			I2C_clock_delay(Continue);
			SCL=1;
			while(SCL!=1);
		}
		// stop condition
		if(return_value!=bus_busy_error)
		{
			I2C_clock_delay(Continue);
			SCL=0;
			SDA=0;
			I2C_clock_delay(Continue);
			SCL=1;
			while(SCL!=1);
			I2C_clock_delay(Stop);
			SDA = 1;
		}	
	}
	return return_value;
}

void I2C_clock_delay(uint8_t control)
{
	if(TR1==1)
	{
		while(TF1==0);
	}
	TR1=0; 
	if(control==Continue)
	{
		TH1=I2C_RELOAD_H;
		TL1=I2C_RELOAD_L;
		TF1=0;
		TR1=1;
	}
}


void I2C_clock_start(void)
{
	TMOD&=0x0F;
	TMOD|=0x10;

	ET1=0;

	TH1 = I2C_RELOAD_H;
	TL1 = I2C_RELOAD_L;
	TF1 = 0;
	TR1 = 1;
}


uint8_t I2C_write(uint8_t device_addr, uint32_t int_addr, uint8_t int_addr_sz, uint8_t num_bytes, uint8_t * send_data_arr)
{
	uint8_t send_val = 0; // byte to send
	uint8_t index = 0;
	uint8_t num_bits = 0;
	uint8_t recv_value = 0; // byte to receive
	bit sent_bit = 0;
	uint8_t return_value = no_errors; // return value with error code
	bit send_bit = 0;

	// Default to 1's before starting writes
	SDA = 1;
	SCL = 1;
	if((SCL==1)&&(SDA==1))
	{
		I2C_clock_start();
		send_val = device_addr<<1; // 7 bit device addr with 0 LSB for write

		// Start Condition
		SDA = 0;
		index = 0;
		num_bits = 8;

		// Send Device Address
		printf("Sending Device addr..\r\n");
		do
		{
			I2C_clock_delay(Continue);
			SCL = 0; // Pull clock low to transition to new bit
			num_bits--;
			send_bit = ((send_val>>num_bits)&0x01); // Shift down and mask off the upper 7 bits
			SDA = send_bit; // Send a single bit
			I2C_clock_delay(Continue);
			SCL=1; // Pull clock high to sample
			while(SCL!=1);
			sent_bit = SDA;

			//Collision Checking
			if(sent_bit!=send_bit)
			{
				return_value=bus_busy_error;
			}
		}while((num_bits!=0)&&(return_value==no_errors));
		printf("Device Addr. Sent\r\n");
		//Wait for ACK/NACK
		if(Ack_check() != ACK)
		{
			return ack_error;
		}
		//Internal Address Check
		if (int_addr_sz > 0)
		{
			num_bits = 8;
			send_val = int_addr;
			//Send Internal Address
			printf("Sending internal Address..\r\n");
			do
			{
				I2C_clock_delay(Continue);
				SCL = 0; // Pull clock low to transition to new bit
				num_bits--;
				send_bit = ((send_val>>num_bits)&0x01);
				SDA = send_bit;
				I2C_clock_delay(Continue);
				SCL=1; // Pull clock high to sample bit
				while(SCL!=1);
				sent_bit = SDA;
				//Collision Checking
				if(sent_bit!=send_bit)
				{
					return_value=bus_busy_error;
				}
			}while((num_bits!=0)&&(return_value==no_errors));
			printf("Internal Address Sent...\r\n");
			//Wait for ACK/NACK
			if(Ack_check() != ACK)
			{
				return ack_error;
			}
		}

		//Data Send, ACK Check Cycle
		while((num_bytes > 0)&&(return_value == no_errors))
		{
			printf("Sending one byte...\r\n");
			num_bits = 8;
			send_val = *(send_data_arr+index);
			do
			{
				I2C_clock_delay(Continue);
				SCL = 0; // Pull clock low to set new bit
				num_bits--;
				send_bit = ((send_val>>num_bits)&0x01);
				SDA = send_bit;
				I2C_clock_delay(Continue);
				SCL=1; // Pull clock high to sample
				while(SCL!=1);

				// collision detection
				sent_bit = SDA;
				if(sent_bit!=send_bit)
				{
					return_value=bus_busy_error;
				}
			}while((num_bits!=0)&&(return_value==no_errors));
			printf("Byte Sent:: %2.2bX \r\n", send_val);
			num_bytes--;
			index++;
			
			//Wait for ACK/NACK
			if(Ack_check() != ACK)
			{
				return ack_error;
			}
		}
		printf("All bytes sent...\r\n");
	}
	// stop condition
	if(return_value!=bus_busy_error)
	{
		I2C_clock_delay(Continue);
		SCL=0;
		SDA=0;
		I2C_clock_delay(Continue);
		SCL=1;
		while(SCL!=1);
		I2C_clock_delay(Stop);
		SDA = 1;
	}
	return return_value;
}
