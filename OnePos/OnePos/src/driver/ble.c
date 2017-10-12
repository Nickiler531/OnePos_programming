/*
 * ble.c
 *
 * Created: 10/11/2017 11:07:15 AM
 *  Author: Nick
 */ 


#include "../include/ble.h"


void ble_init(void)
{
	
	static usart_rs232_options_t BLE_SERIAL_OPTIONS = {
		.baudrate = USART_BLE_BAUDRATE,
		.charlength = USART_BLE_CHAR_LENGTH,
		.paritytype = USART_BLE_PARITY,
		.stopbits = USART_BLE_STOP_BIT
	};
	usart_serial_init(USART_BLE, &BLE_SERIAL_OPTIONS);
}



void usart_read_string(char * str)
{
	uint8_t flag = 1;
	volatile uint16_t counter = 0;
	while(flag)
	{
		if (usart_rx_is_complete(USART_BLE))
		{
			*str = usart_get(USART_BLE);
			str++;
			counter=0;
		}
		else
		{
			counter++;
			asm("nop");
			
		}
		
		if (counter > 10000 )
		{
			flag = 0;
			*str = '\0';
			led3_toogle();
		}
		
	}
}

void usart_write_string(char * str)
{
	while(*str!=0)
	{
		usart_putchar(USART_BLE,*str);
		str++;
	}
}

void ble_conf_readables(void)
{
	
}

uint8_t ble_information(char * ble_str, beacon_struct * beacon)
{
	uint8_t size = strlen(ble_str);
	char rssi[3] = "";
	char tx_power[3] = "";
	int aux = 0;
	
	if (size == 80)
	{
		strncpy(beacon->mac,ble_str+1,12);
		strncpy(beacon->namespace, ble_str+45,20);
		strncpy(beacon->instanceID, ble_str+65,12);
		strncpy(rssi, ble_str+16,2);
		strncpy(tx_power, ble_str+43,2);
		
		sscanf(rssi,"%x",&aux);
		beacon->rssi = (int8_t)aux;
		sscanf(tx_power,"%x",&aux);
		beacon->tx_power = (int8_t)aux;
		
		printf("------------------------\n");
		printf("MAC         : %s\n",beacon->mac);
		printf("NamespaceID : %s\n",beacon->namespace);
		printf("Instance ID : %s\n",beacon->instanceID);
		printf("RSSI        : %d\n",beacon->rssi);
		printf("TX Power    : %d\n",beacon->tx_power);
		printf("------------------------\n");
		return 1;
	}
	return 0;
}

void ble_init_read_beacons(void)
{
	char buffer[100];
	usart_write_string(BLE_CMD_MODE);
	usart_read_string(buffer);
	printf("%s\n",buffer);
	
	delay_ms(200);
	usart_write_string("F\n"); //ToDo Add parameters of initial configuration
}

uint8_t ble_read_beacons(beacon_struct * beacon)
{
	char buffer[500];
	
	if (usart_rx_is_complete(USART_BLE))
	{
		usart_read_string(buffer);
		printf("%s\n",buffer);
		return ble_information(buffer,beacon);
	}
	return 0;
}
