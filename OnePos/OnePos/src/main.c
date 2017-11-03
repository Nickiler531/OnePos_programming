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


void rs485_init(void)
{
	static usart_rs232_options_t RS485_SERIAL_OPTIONS = {
		.baudrate = USART_SERIAL_BAUDRATE,
		.charlength = USART_SERIAL_CHAR_LENGTH,
		.paritytype = USART_SERIAL_PARITY,
		.stopbits = USART_SERIAL_STOP_BIT
	};
	
	usart_serial_init(USART_RS485, &RS485_SERIAL_OPTIONS);
	ioport_set_pin_level(RS485_TX_EN,0);
}

void rs485_read_string(char * str)
{
	uint8_t flag = 1;
	volatile uint16_t counter = 0;
	
	ioport_set_pin_level(RS485_TX_EN,0);
	
	while(flag)
	{
		if (usart_rx_is_complete(USART_RS485))
		{
			*str = usart_get(USART_RS485);
			str++;
			counter=0;
		}
		else
		{
			counter++;
			asm("nop");
		}
		
		if (counter > 0x0FFF )
		{
			flag = 0;
			*str = '\0';
			led3_toogle();
		}
	}
}

void rs485_write_string(char * str)
{
	ioport_set_pin_level(RS485_TX_EN,1);
	delay_ms(1);
	
	while(*str!=0)
	{
		usart_putchar(USART_RS485,*str);
		str++;
	}
	delay_ms(5);
	ioport_set_pin_level(RS485_TX_EN,0);
}

void send_pos_info(int16_t node1_rssi, uint16_t node1_distance, int16_t node2_rssi, uint16_t node2_distance, int16_t node3_rssi, int16_t node3_distance, int16_t node4_rssi, uint16_t node4_distance)
{
	char buffer[120];
	sprintf(buffer,"{\"rssi1\":%d,\"d1\":%d, \"rssi2\":%d,\"d2\":%d, \"rssi3\":%d,\"d3\":%d, \"rssi4\":%d,\"d4\":%d}", node1_rssi, node1_distance, node2_rssi, node2_distance, node3_rssi, node3_distance, node4_rssi, node4_distance);
	rs485_write_string(buffer);
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
	
	if (e.var == 0x0A0B0C0D )
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
#include "include/deca_examples.h"

#define BLE_RX_BUFFER 90
char aux;
char ble_buffer[BLE_RX_BUFFER];
uint8_t ble_buff_index = 0;
beacon_struct ble_beacon;
ISR(USARTD0_RXC_vect)
{
	led4_toogle();
	aux = usart_get(USART_BLE);
	ble_buffer[ble_buff_index] = aux;
	#ifdef DBG
	usb_putchar(aux);
	#endif
	ble_buff_index++;
	if (aux == '\n')
	{
		led3_toogle();
		ble_buffer[ble_buff_index] = '\0';
		if(ble_information(ble_buffer,&ble_beacon))
		{
			printf("%d\n",ble_beacon.rssi);
		}
		ble_buff_index = 0;
	}
	if (ble_buff_index == BLE_RX_BUFFER-1)
	{
		ble_buff_index = 0;
		led2_toogle();
	}
}

void base_node(void)
{
	uint16_t dist;
	uint8_t status = -1;
	uint16_t distances[4] = {0,0,0,0};
	int16_t rssis[4] = {0,0,0,0};
	char msg[20];
	
	printf("\nNode configured as Base Node\n");
	
	ble_init_read_beacons_isr();
	
	for (;;)
	{
		status = dwt_resp_twr(&distances[0]);
		rssis[0] = ble_beacon.rssi;
		if (status == UWB_RX_OK)
		{
			led1_toogle();
			dwt_send_msg_w_ack("msg",4);
			status = dwt_receive_msg_w_ack(msg);
			if (status == UWB_RX_OK)
			{
				led4_toogle();
				printf("4.............\n");
				sscanf(msg,"%d,%d",&distances[3],&rssis[3]);
				status = -1;
			}
			dwt_send_msg_w_ack("msg",3);
			status = dwt_receive_msg_w_ack(msg);
			if (status == UWB_RX_OK)
			{
				led3_toogle();
				printf("3..................\n");
				sscanf(msg,"%d,%d",&distances[2],&rssis[2]);
				status = -1;
			}
			dwt_send_msg_w_ack("msg",2);
			status = dwt_receive_msg_w_ack(msg);
			if (status == UWB_RX_OK)
			{
				led2_toogle();
				printf("2................\n");
				sscanf(msg,"%d,%d",&distances[1],&rssis[1]);
				status = -1;
			}
			
			printf("\t->d1: %d\n\t->d2: %d\n\t->d3: %d\n\t->d4: %d\n",distances[0],distances[1],distances[2],distances[3]);
			printf("\t->rssi1:%d\n\t->rssi2:%d\n\t->rssi3:%d\n\t->rssi4:%d\n",rssis[0],rssis[1],rssis[2],rssis[3]);
			send_pos_info(rssis[0],distances[0],rssis[1],distances[1],rssis[2],distances[2],rssis[3],distances[0]);
			
			distances[0] = 0;
			distances[1] = 0;
			distances[2] = 0;
			distances[3] = 0;
			rssis[0] = 0;
			rssis[1] = 0;
			rssis[2] = 0;
			rssis[3] = 0;
		}
	}
}

void location_node(void)
{
	
	printf("\nNode configured as Location Node\n");
	uint8_t status;
	for (;;)
	{
		status = dwt_init_twr(4);
		if (status == UWB_RX_OK)
		{
			led4_toogle();
		}
		status = dwt_init_twr(3);
		if (status == UWB_RX_OK)
		{
			led3_toogle();
		}
		status = dwt_init_twr(2);
		if (status == UWB_RX_OK)
		{
			led2_toogle();
		}
		status = dwt_init_twr(1);
		if (status == UWB_RX_OK)
		{
			led1_toogle();
		}
		delay_ms(1000);
	}
}

void support_node(void)
{
	beacon_struct current_beacon;
	uint8_t status = -1;
	uint8_t status2 = -1;
	uint16_t dist;
	char msg[15];
	
	printf("\nNode configured as Support Node\n");
	ble_init_read_beacons_isr();
	
	for(;;)
	{
		led1(OFF);
		led2(OFF);
		status = dwt_resp_twr(&dist);
		while(status == UWB_RX_OK)
		{
			led1(ON);
			status2 = dwt_receive_msg_w_ack(msg);
			if (status2 == UWB_RX_OK)
			{
				led2_toogle();
				cli();
				sprintf(msg,"%d,%d",dist,ble_beacon.rssi);
				sei();
				dwt_send_msg_w_ack(msg,1);
				status = -1;
				status2 = -1;
			}
		}
	}
}


int main (void)
{
	init_onepos();
	
	rs485_init();
	ble_init();
	usart_set_rx_interrupt_level(USART_BLE, USART_INT_LVL_LO);
	
	init_animation();
	
	uint8_t ret;
	ret = onepos_read_cfg();
	printf("READ_CFG: %d. MEMCHECK = %x\n",ret,onepos_get_mem_check());
	
	if (sw2_status())
	{
		onepos_configure_interface();
	}
	else
	{
		onepos_print_current_configuration();
	}
	
	dwt_onepos_init(1);
	
	//support_node();
	base_node();
	//location_node();
	
	
	uint16_t node1 = 1;
	uint16_t node2 = 3;
	uint16_t node3 = 5;
	
	//dwt_node1_calibration(node2,node3);
	//dwt_node2_calibration(node1,node3);
	//dwt_node3_calibration(node1,node2);

	
	
	//for(;;)
	//{
		//send_pos_info(10,11,22,23,30,33,44,47);
		//delay_ms(2000);
	//}
	
	//char str[50];
	//for(;;)
	//{
		//led2_toogle();
		//if(usart_rx_is_complete(USART_RS485))
		//{
			//led1_toogle();
			//rs485_read_string(str);
			//delay_ms(200);
			//rs485_write_string(str);
		//}
	//}

	//for(;;) //USB-USART_BLE bridge
	//{
		//if (usb_is_rx_ready())
		//{
			//usart_putchar(USART_BLE, usb_getchar());
			//led1_toogle();
		//}
		//if (usart_rx_is_complete(USART_BLE))
		//{
			//usb_putchar(usart_getchar(USART_BLE));
			//led2_toogle();
		//}
		//
	//}
	//
	//
	////Test of sending messages with library D:
	//dwt_onepos_init(1);
	//char income_msg[125];
	//uint16_t distance;
	//for (;;) //INIT TWR
	//{
			//
		//while (dwt_init_twr(1) != UWB_RX_OK)
		//{
			//delay_ms(1000);
		//}
		//
		//while(dwt_receive_msg_w_ack(income_msg) != UWB_RX_OK);
		//
		//printf("SUCCESS!!!\n");
		//printf("%s\n",income_msg);
		//delay_ms(500);
		//
	//}
	//
	//for (;;) //RESP TWR
	//{
		//while(dwt_resp_twr(&distance) != UWB_RX_OK);
		//sprintf(income_msg,"distance = %d\n",distance);
		//delay_ms(200);
		//if(dwt_send_msg_w_ack(income_msg,uwb_rx_message.source_address) == UWB_RX_OK)
		//{
			//printf("%s\n",income_msg);
			//printf("SUCCESS!!!\n");
		//}
		//else
		//{
			//printf("FAILURE\n");
		//}
		//
	//}
	//
	//
	//
	//
	//for (;;)
	//{
		//led1_toogle();
		//delay_ms(500);
	//}
	//
	//
	//
	//uint32_t dev_id;
	//
	//for (;;) //spi test
	//{
		//dev_id = dwt_readdevid();
		//printf("%#04X %#04X\n", (uint16_t)(dev_id>>16),(uint16_t)dev_id);
		//led1_toogle();
		//delay_ms(1000);
	//}
	//
	//
	//
	//
	//for (;;) //Basic USB printf example
	//{
		//printf("hello!\n");
		//delay_ms(500);
	//}
}
