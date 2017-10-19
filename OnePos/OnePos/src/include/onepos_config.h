/*
 * onepos_config.h
 *
 * Created: 10/10/2017 9:25:52 PM
 *  Author: Nick
 */ 



#ifndef ONEPOS_CONFIG_H_
#define ONEPOS_CONFIG_H_

#include "../onepos.h"

#define MEM_CHECK_CONSTANT 0x10C8 //Locate
#define EEPROM_START_ADDRESS 0x1000

/**
 * @brief      Enum used to choose the start process
 */
enum node_mode_enum
{
	START_AS_BEACON,
	START_AS_ONEPOS
};

enum onepos_config_return_status
{
	OP_CFG_OK,
	OP_CFG_NEWNODE
};

#define DEFAULT_NODEID 0x66
#define DEFAULT_NODEMODE START_AS_ONEPOS
#define DEFAULT_NAMESPACE "6F234454CF6D4A0FADF2"
#define DEFAULT_BLE_SCAN_INTERVAL "01E0"
#define DEFAULT_BLE_WINDOW_INTERVAL "0190"
#define DEFAULT_UWB_RX_ANTENNA_DELAY 16436
#define DEFAULT_UWB_TX_ANTENNA_DELAY 16436

typedef struct onepos_cfg_str
{
	uint16_t mem_check; //This is used to see if the memory is being written for the first time
	uint16_t node_id; //ID of the node. Is used for the identification on the UWB messages
	uint8_t node_mode; //Default mode for the node to start. Could START_AS_BEACON or START_AS_ONEPOS
	char namespaceID[21]; //String with the namespace value represented in ASCII. Letter must be in caps.
	char ble_scan_interval[5]; //string with the hex16 value of the ble scan interval
	char ble_scan_window[5]; //string with the hex16 value of the ble window interval
	uint16_t uwb_rx_antenna_delay; //the delay of the antenna calculated on the calibration process
	uint16_t uwb_tx_antenna_delay; //the delay of the antenna calculated on the calibration process
}onepos_cfg_str; 


volatile onepos_cfg_str onepos_config;

/**
 * @brief      { function_description }
 *
 * @param[in]  config  The configuration
 *
 * @return     { description_of_the_return_value }
 */
uint8_t onepos_read_cfg(void);

/**
 * @brief      { function_description }
 *
 * @param      config  The configuration
 *
 * @return     { description_of_the_return_value }
 */
void onepos_save_cfg(void);

uint8_t onepos_write_default_cfg(void);

uint8_t onepos_configure_interface(void);

uint16_t onepos_get_mem_check(void);

uint16_t onepos_get_node_id(void);

uint8_t onepos_get_node_mode(void);

void onepos_get_namespaceID(char * str);

void onepos_get_ble_scan_interval(char * str);

void onepos_get_ble_scan_window(char * str);

void onepos_set_node_id(uint16_t node_id);

void onepos_set_node_mode(uint8_t node_mode);

void onepos_set_namespaceID(char * str);

void onepos_set_ble_scan_interval(char * str);

void onepos_set_ble_scan_window(char * str);

uint16_t onepos_get_uwb_rx_antenna_delay(void);

void onepos_set_uwb_rx_antenna_delay(uint16_t delay);

uint16_t onepos_get_uwb_tx_antenna_delay(void);

void onepos_set_uwb_tx_antenna_delay(uint16_t delay);



#endif /* ONEPOS_CONFIG_H_ */