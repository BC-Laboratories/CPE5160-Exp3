#include "Main.h"
#include "I2C.h"
#include <stdio.h>

sbit SDA = P1^0;
sbit SCL = P1^1;

uint8_t Ack_Check(void)
{
	bit response_bit;
	I2C_clock_delay(Continue);
	SCL = 0;
	SDA = 1;
	I2C_clock_delay(Continue);
	SCL = 1;
	while(SCL != 1);
	response_bit = SDA;
	if(response_bit != 1)
	{
		printf("Received Ack...\r\n");
		return ACK;
	}
	else
	{
		printf("Received Nack\r\n");
		return NACK;
	}
}

uint8_t I2C_read(uint8_t device_addr, uint32_t internal_addr, uint8_t int_size, uint8_t num_bytes, uint8_t * store_data_arr)
{
	uint8_t send_val = 0;
	uint8_t index = 0;
	uint8_t num_bits = 0;
	uint8_t recv_value = 0;
	bit sent_bit = 0;
	uint8_t return_value = no_errors;
	bit send_bit = 0;

	SDA = 1;
	SCL = 1;

	if((SCL==1)&&(SDA==1))
	{
		I2C_clock_start();
		send_val = device_addr << 1;
		send_val |= 0x01; // Set R/W bit
		// Send Start
		SDA = 0;
		index = 0;
		
		//send device addr
		printf("Sending device addr.\r\n");
		num_bits = 8;
		do
		{
			I2C_clock_delay(Continue);
			SCL = 0;
			num_bits--;
			send_bit = ((send_val>>num_bits)&0x01);
			SDA = send_bit;
			I2C_clock_delay(Continue);
			SCL=1;
			while(SCL!=1);
			// collision detection
			sent_bit = SDA;
			if(sent_bit!=send_bit)
			{
				return_value=bus_busy_error;
			}
		}while((num_bits!=0)&&(return_value==no_errors));
		
		//Wait for ACK/NACK
		if(Ack_check() != ACK)
		{
			return ack_error;
		}
		// read from address
		while((num_bytes>0)&&(return_value == no_errors))
		{
			num_bits = 8;
			do
			{
				I2C_clock_delay(Continue);
				SCL=0;
				num_bits--;
				SDA = 1; // Set input to be 1
				recv_value = recv_value << 1;
				I2C_clock_delay(Continue);
				SCL=1;
				while(SCL!=1);
				sent_bit = SDA;
				recv_value |= sent_bit;
			}while(num_bits!=0);
			*(store_data_arr+index) = recv_value;
			index++;
			num_bytes--;
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
	printf("I made it here!");
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
	uint8_t send_val = 0;
	uint8_t index = 0;
	uint8_t num_bits = 0;
	uint8_t recv_value = 0;
	bit sent_bit = 0;
	uint8_t return_value = no_errors;
	bit send_bit = 0;

	return_value = no_errors;
	SDA = 1;
	SCL = 1;
	if((SCL==1)&&(SDA==1))
	{
		I2C_clock_start();
		send_val = device_addr<<1;
		//leaves LSB as a 0 for write
		
		//send start
		SDA = 0;
		index = 0;
		num_bits = 8;
		//Send Device Address
		do
		{
			I2C_clock_delay(Continue);
			SCL = 0;
			num_bits--;
			send_bit = ((send_val>>num_bits)&0x01);
			SDA = send_bit;
			I2C_clock_delay(Continue);
			SCL=1;
			while(SCL!=1);
			sent_bit = SDA;
			//Collision Checking
			if(sent_bit!=send_bit)
			{
				return_value=bus_busy_error;
			}
		}while((num_bits!=0)&&(return_value==no_errors));
		
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
			do
			{
				I2C_clock_delay(Continue);
				SCL = 0;
				num_bits--;
				send_bit = ((send_val>>num_bits)&0x01);
				SDA = send_bit;
				I2C_clock_delay(Continue);
				SCL=1;
				while(SCL!=1);
				sent_bit = SDA;
				//Collision Checking
				if(sent_bit!=send_bit)
				{
					return_value=bus_busy_error;
				}
			}while((num_bits!=0)&&(return_value==no_errors));
		
			//Wait for ACK/NACK
			if(Ack_check() != ACK)
			{
				return ack_error;
			}
		}


		//Data Send, ACK Check Cycle
		while((num_bytes > 0)&&(return_value == no_errors))
		{

			num_bits = 8;
			send_val = *(send_data_arr+index);
			do
			{
				I2C_clock_delay(Continue);
				SCL = 0;
				send_bit = ((send_val>>num_bits)&0x01);
				SDA = send_bit;
				I2C_clock_delay(Continue);
				SCL=1;
				while(SCL!=1);

				// collision detection
				sent_bit = SDA;
				if(sent_bit!=send_bit)
				{
					return_value=bus_busy_error;
				}
			}while((num_bits!=0)&&(return_value==no_errors));
			
			num_bytes--;
			index++;
			
			//Wait for ACK/NACK
			if(Ack_check() != ACK)
			{
				return ack_error;
			}

		}


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
