#include <stdio.h>
#include "Main.h"
#include "PORT.H"
#include "UART.h"
#include "Timer0_hardware_delay_1ms.h"
#include "LCD_routines.h"
#include "LED_Outputs.h"
#include "I2C.h"
#include "STA013.h"

uint8_t code exp3_string[]="STA013 Init";


main()
{
   uint8_t input_value;

   LEDS_ON(Red_LED);
   AUXR=0x0c;   // make all of XRAM available, ALE always on
   if(OSC_PER_INST==6)
   {
       CKCON0=0x01;  // set X2 clock mode
   }
   else if(OSC_PER_INST==12)
   {
       CKCON0=0x00;  // set standard clock mode
   }
   Timer0_DELAY_1ms(300);
   uart_init(9600);
   LEDS_OFF(Red_LED);
   

   LCD_Init();  
   LCD_Print(line1,0,exp3_string);
   STA013_init();
   while(1)
   {

	    input_value=UART_Receive();
        UART_Transmit(input_value);

   } 
}


