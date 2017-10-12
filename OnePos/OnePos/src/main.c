/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
 /**
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
#include "onepos.h"

#define FALSE 0
#define TRUE 1
 
#define USART_RS485						 &USARTE0
#define USART_SERIAL_BAUDRATE            115200
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false



void rs485_send(char a)
{
	
}







void test_endian(void)
{
	union e {
		unsigned long int var;
		unsigned char array[4];
	}e;
	
	e.array[0] = 0x0D;
	e.array[1] = 0x0C;
	e.array[2] = 0x0B;
	e.array[3] = 0x0A;
	
	if ( e.var == 0x0A0B0C0D )
	{
		printf("Little-endian\n\r");
	}
	else if (e.var == 0x0D0C0B0A)
	{
		printf("Big-endian\n\r");
	}
	else
	{
		printf("Middle-endian or unknown storage type. Variable= %x", e.var);
	}
}

int main (void)
{
	init_onepos();
	openspi();
	
	static usart_rs232_options_t RS485_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_RS485, &RS485_SERIAL_OPTIONS);
	
	init_animation();
	
	dwt_show_sys_info();
	

	delay_ms(200);
	
	dwt_run_examples();
	
	uint8_t ret;
	
	
	
	ret = onepos_read_cfg();
	printf("READ_CFG: %d. MEMCHECK = %x\n",ret,onepos_get_mem_check());
	onepos_configure_interface();
	
	
	
	
	for (;;)
	{
		led1_toogle();
		delay_ms(500);
	}
	
	for(;;) //USB-USART_BLE bridge
	{
		if (usb_is_rx_ready())
		{
			usart_putchar(USART_BLE, usb_getchar());
			led1_toogle();
		}
		if (usart_rx_is_complete(USART_BLE))
		{
			usb_putchar(usart_getchar(USART_BLE));
			led2_toogle();
		}
		
	}
	
	uint32_t dev_id;
	
	for (;;) //spi test
	{
		dev_id = dwt_readdevid();
		printf("%#04X %#04X\n", (uint16_t)(dev_id>>16),(uint16_t)dev_id);
		led1_toogle();
		delay_ms(1000);
	}
	
	
	
	
	for (;;) //Basic USB printf example
	{
		printf("hello!\n");
		delay_ms(500);
	}
}
