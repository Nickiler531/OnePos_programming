/*
 * ble.h
 *
 * Created: 10/11/2017 11:07:04 AM
 *  Author: Nick
 */ 


#ifndef BLE_H_
#define BLE_H_


#include "../onepos.h"

#define USART_BLE                     &USARTD0
#define USART_BLE_BAUDRATE            115200
#define USART_BLE_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_BLE_PARITY              USART_PMODE_DISABLED_gc
#define USART_BLE_STOP_BIT            false


//Command list

#define BLE_CMD_MODE "$$$"
#define BLE_DATA_MODE "---"
#define BLE_DISPAY_INFO "D"
#define BLE_START_SCANNING "F" 
#define BLE_STOP_SCANNING "X"


typedef struct beacon_struct
{
	char mac[13];
	char namespaceID[21];
	char instanceID[13];
	int rssi;
	int tx_power;	
}beacon_struct;

void ble_init(void);
void usart_read_string(char * str);
void ble_init_read_beacons_isr(void);
void usart_write_string(char * str);
void ble_conf_readables(void); // ToDo
uint8_t ble_information(char * ble_str, beacon_struct * beacon);

void ble_init_read_beacons(void);
uint8_t ble_read_beacons(beacon_struct * beacon);

uint8_t ble_read_beacons_isr(beacon_struct * beacon);

#endif /* BLE_H_ */