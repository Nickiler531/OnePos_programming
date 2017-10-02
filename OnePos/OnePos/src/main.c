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


#define USART_RS485						 &USARTE0
#define USART_BLE                        &USARTD0
#define USART_SERIAL_BAUDRATE            115200
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false


void spi_init_module(void)
{
	struct spi_device spi_device_conf = {
		.id = UWB_CS
	};
	spi_master_init(&SPIC);
	spi_master_setup_device(&SPIC, &spi_device_conf, SPI_MODE_0, 1000000, 0);
	spi_enable(&SPIC);
}

void rs485_send(char a)
{
	
}

int main (void)
{
	init_onepos();
	spi_init_module();
	
	static usart_rs232_options_t BLE_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_BLE, &BLE_SERIAL_OPTIONS);
	
	static usart_rs232_options_t RS485_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	usart_serial_init(USART_RS485, &RS485_SERIAL_OPTIONS);
	
	
	init_animation();
	
	uint8_t data_send[1] = {0x00};
	uint8_t data_receive[4];
	struct spi_device spi_device_conf = {
		.id = UWB_CS
	};
	
	for (;;) //spi test
	{
		spi_select_device(&SPIC, &spi_device_conf);
		spi_write_packet(&SPIC, data_send, 1);
		spi_read_packet(&SPIC, data_receive, 4);
		spi_deselect_device(&SPIC, &spi_device_conf);
		delay_ms(1000);
		printf("%#02X %#02X %#02X %#02X \n", data_receive[3], data_receive[2], data_receive[1], data_receive[0]);
		led1_toogle();
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
	
	for (;;) //Basic USB printf example
	{
		printf("hello!\n");
		delay_ms(500);
	}
}
