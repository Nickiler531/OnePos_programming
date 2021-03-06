/*
 * onepos_config.c
 *
 * Created: 10/10/2017 9:25:08 PM
 *  Author: Nick
 */ 

#include "../include/onepos_config.h"

uint8_t onepos_read_cfg(void)
{
	nvm_eeprom_read_buffer(EEPROM_START,&onepos_config,sizeof(onepos_cfg_str));
	if (onepos_config.mem_check == MEM_CHECK_CONSTANT)
	{
		return OP_CFG_OK;
	}
	onepos_write_default_cfg();
	return OP_CFG_NEWNODE;
}

void onepos_save_cfg(void)
{
	nvm_eeprom_erase_and_write_buffer(EEPROM_START,&onepos_config,sizeof(onepos_cfg_str));
}

uint8_t onepos_write_default_cfg(void)
{
	onepos_config.mem_check = MEM_CHECK_CONSTANT;
	onepos_config.node_id = onepos_get_node_id();
	onepos_config.node_mode = DEFAULT_NODEMODE;
	strcpy(onepos_config.namespaceID,DEFAULT_NAMESPACE);
	strcpy(onepos_config.ble_scan_interval,DEFAULT_BLE_SCAN_INTERVAL);
	strcpy(onepos_config.ble_scan_window,DEFAULT_BLE_WINDOW_INTERVAL);
	switch(onepos_config.node_id)
	{
		case 1:
			onepos_config.uwb_rx_antenna_delay = UWB_RX_ANT_DLY_N1;
			onepos_config.uwb_tx_antenna_delay = UWB_TX_ANT_DLY_N1;
		break;
		case 2:
			onepos_config.uwb_rx_antenna_delay = UWB_RX_ANT_DLY_N2;
			onepos_config.uwb_tx_antenna_delay = UWB_TX_ANT_DLY_N2;
		break;
		case 3:
			onepos_config.uwb_rx_antenna_delay = UWB_RX_ANT_DLY_N3;
			onepos_config.uwb_tx_antenna_delay = UWB_TX_ANT_DLY_N3;
		break;
		case 4:
			onepos_config.uwb_rx_antenna_delay = UWB_RX_ANT_DLY_N4;
			onepos_config.uwb_tx_antenna_delay = UWB_TX_ANT_DLY_N4;
		break;
		case 5:
			onepos_config.uwb_rx_antenna_delay = UWB_RX_ANT_DLY_N5;
			onepos_config.uwb_tx_antenna_delay = UWB_TX_ANT_DLY_N5;
		break;
		default:
			onepos_config.uwb_rx_antenna_delay = DEFAULT_UWB_RX_ANTENNA_DELAY;
			onepos_config.uwb_tx_antenna_delay = DEFAULT_UWB_TX_ANTENNA_DELAY;
	}
	
	onepos_save_cfg();
}

void onepos_print_current_configuration(void)
{
	char namespaceID[21];
	char ble_scan_interval[5];
	char ble_scan_window[5];
	
	onepos_get_namespaceID(namespaceID);
	onepos_get_ble_scan_interval(ble_scan_interval);
	onepos_get_ble_scan_window(ble_scan_window);
	
	//printf("    ,o888888o.     b.             8 8 8888888888   8 888888888o   \n");
	//printf(" . 8888     `88.   888o.          8 8 8888         8 8888    `88. \n");
	//printf(",8 8888       `8b  Y88888o.       8 8 8888         8 8888     `88 \n");
	//printf("88 8888        `8b .`Y888888o.    8 8 8888         8 8888     ,88 \n");
	//printf("88 8888         88 8o. `Y888888o. 8 8 888888888888 8 8888.   ,88' \n");
	//printf("88 8888         88 8`Y8o. `Y88888o8 8 8888         8 888888888P'  \n");
	//printf("88 8888        ,8P 8   `Y8o. `Y8888 8 8888         8 8888         \n");
	//printf("`8 8888       ,8P  8      `Y8o. `Y8 8 8888         8 8888         \n");
	//printf(" ` 8888     ,88'   8         `Y8o.` 8 8888         8 8888         \n");
	//printf("    `8888888P'     8            `Yo 8 888888888888 8 8888         \n");
	//printf("    ,o888888o.     b.             8 8 8888888888   8 888888888o       ,o888888o.       d888888o.   \n");
	//printf(" . 8888     `88.   888o.          8 8 8888         8 8888    `88.  . 8888     `88.   .`8888:' `88. \n");
	//printf(",8 8888       `8b  Y88888o.       8 8 8888         8 8888     `88 ,8 8888       `8b  8.`8888.   Y8 \n");
	//printf("88 8888        `8b .`Y888888o.    8 8 8888         8 8888     ,88 88 8888        `8b `8.`8888.     \n");
	//printf("88 8888         88 8o. `Y888888o. 8 8 888888888888 8 8888.   ,88' 88 8888         88  `8.`8888.    \n");
	//printf("88 8888         88 8`Y8o. `Y88888o8 8 8888         8 888888888P'  88 8888         88   `8.`8888.   \n");
	//printf("88 8888        ,8P 8   `Y8o. `Y8888 8 8888         8 8888         88 8888        ,8P    `8.`8888.  \n");
	//printf("`8 8888       ,8P  8      `Y8o. `Y8 8 8888         8 8888         `8 8888       ,8P 8b   `8.`8888. \n");
	//printf(" ` 8888     ,88'   8         `Y8o.` 8 8888         8 8888          ` 8888     ,88'  `8b.  ;8.`8888 \n");
	//printf("    `8888888P'     8            `Yo 8 888888888888 8 8888             `8888888P'     `Y8888P ,88P' \n");
	printf("\nCurrent Configuration: \n");
	printf("MEM CHECK         : %#x\n",onepos_get_mem_check());
	printf("NODE ID           : %d\n",onepos_get_node_id());
	printf("NODE MODE         : %s\n",onepos_get_node_mode() ? "Start as OnePos" : "Start as Beacon" );
	printf("NAMESPACE ID      : %s\n",namespaceID);
	printf("BLE SCAN INTERVAL : %s\n",ble_scan_interval);
	printf("BLE SCAN WINDOW   : %s\n",ble_scan_window);
	printf("UWB RX ANT DELAY  : %d\n",onepos_get_uwb_rx_antenna_delay());
	printf("UWB TX ANT DELAY  : %d\n",onepos_get_uwb_tx_antenna_delay());
}

static print_menu1(void)
{
	char namespaceID[21];
	char ble_scan_interval[5];
	char ble_scan_window[5];
	
	onepos_get_namespaceID(namespaceID);
	onepos_get_ble_scan_interval(ble_scan_interval);
	onepos_get_ble_scan_window(ble_scan_window);
	
	printf("\n-----------------------\n");
	printf("Current Configuration: \n");
	printf("   MEM CHECK         : %#x\n",onepos_get_mem_check());
	printf("1. NODE ID           : %d\n",onepos_get_node_id());
	printf("2. NODE MODE         : %s\n",onepos_get_node_mode() ? "Start as OnePos" : "Start as Beacon" );
	printf("3. NAMESPACE ID      : %s\n",namespaceID);
	printf("4. BLE SCAN INTERVAL : %s\n",ble_scan_interval);
	printf("5. BLE SCAN WINDOW   : %s\n",ble_scan_window);
	printf("6. UWB RX ANT DELAY  : %d\n",onepos_get_uwb_rx_antenna_delay());
	printf("7. UWB TX ANT DELAY  : %d\n",onepos_get_uwb_tx_antenna_delay());
	printf("0. SAVE AND EXIT\n");
	printf("Select a number to change the current configuration: ");
}

uint8_t onepos_configure_interface(void)
{
	char new_namespaceID[21];
	char new_ble_scan_interval[5]; 
	char new_ble_scan_window[5]; 
	uint8_t new_node_id=0;
	uint8_t new_node_mode=0;
	uint8_t end_config = 0;
	uint8_t selection;
	uint16_t new_uwb_rx_antenna_delay = 0;
	uint16_t new_uwb_tx_antenna_delay = 0;
	
	while(!end_config)
	{
		print_menu1();
		scanf("%d", &selection);
		
		switch(selection)
		{
			case 1:
				printf("\nTo change node ID program the user signature memory at address 0x00\n");
				delay_ms(2000);
			break;
			case 2:
				printf("\nWrite 0 to start as Beacon and 1 to start as OnePos: ");
				scanf("%d",&new_node_mode);
				if ( !(new_node_mode == 0 | new_node_id == 1) )
				{
					new_node_mode = 1;
				}
				onepos_set_node_mode(new_node_mode);
			break;
			case 3:
				printf("\nWrite the new namespace ID: ");
				scanf("%20s",new_namespaceID);
				onepos_set_namespaceID(new_namespaceID);
			break;
			case 4:
				printf("\nWrite the new ble scan interval(16 hex): ");
				scanf("%4s",new_ble_scan_interval);
				onepos_set_ble_scan_interval(new_ble_scan_interval);
			break;
			case 5:
				printf("\nWrite the new ble scan window( 16 hex): ");
				scanf("%4s",new_ble_scan_window);
				onepos_set_ble_scan_window(new_ble_scan_window);
			break;
			case 6:
				printf("\nWrite the new uwb rx antenna delay: ");
				scanf("%ld",&new_uwb_rx_antenna_delay);
				onepos_set_uwb_rx_antenna_delay(new_uwb_rx_antenna_delay);
			break;
			case 7:
				printf("\nWrite the new uwb tx antenna delay: ");
				scanf("%ld",&new_uwb_tx_antenna_delay);
				onepos_set_uwb_tx_antenna_delay(new_uwb_tx_antenna_delay);
			break;
			case 0:
				onepos_save_cfg();
				printf("\nConfiguration saved. Please restart the system\n");
				end_config = 1;
			break;
		}
	}
	
	
}

uint16_t onepos_get_mem_check(void)
{
	return onepos_config.mem_check;
}

uint16_t onepos_get_node_id(void)
{
	return nvm_read_user_signature_row(NODE_ID_ADDR);
}

uint8_t onepos_get_node_mode(void)
{
	return onepos_config.node_mode;
}

void onepos_get_namespaceID(char * str)
{
	strcpy(str,onepos_config.namespaceID);
}

void onepos_get_ble_scan_interval(char * str)
{
	strcpy(str,onepos_config.ble_scan_interval);
}

void onepos_get_ble_scan_window(char * str)
{
	strcpy(str,onepos_config.ble_scan_window);
}

//void onepos_set_node_id(uint16_t node_id)
//{
	//onepos_config.node_id = node_id;
//}

void onepos_set_node_mode(uint8_t node_mode)
{
	onepos_config.node_mode = node_mode;
}

void onepos_set_namespaceID(char * str)
{
	strcpy(onepos_config.namespaceID,str);
}

void onepos_set_ble_scan_interval(char * str)
{
	strcpy(onepos_config.ble_scan_interval,str);
}

void onepos_set_ble_scan_window(char * str)
{
	strcpy(onepos_config.ble_scan_window,str);
}

uint16_t onepos_get_uwb_rx_antenna_delay(void)
{
	return onepos_config.uwb_rx_antenna_delay;
}

void onepos_set_uwb_rx_antenna_delay(uint16_t delay)
{
	onepos_config.uwb_rx_antenna_delay = delay;
}

uint16_t onepos_get_uwb_tx_antenna_delay(void)
{
	return onepos_config.uwb_tx_antenna_delay;
}

void onepos_set_uwb_tx_antenna_delay(uint16_t delay)
{
	onepos_config.uwb_tx_antenna_delay = delay;
}