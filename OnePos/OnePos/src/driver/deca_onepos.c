/*
 * deca_onepos.c
 *
 * Created: 10/12/2017 5:44:54 PM
 *  Author: CEA-IoT_NVS
 */ 

#include "../include/deca_onepos.h"

dwt_config_t config = {
	2,               /* Channel number. */
	DWT_PRF_64M,     /* Pulse repetition frequency. */
	DWT_PLEN_1024,   /* Preamble length. Used in TX only. */
	DWT_PAC32,       /* Preamble acquisition chunk size. Used in RX only. */
	9,               /* TX preamble code. Used in TX only. */
	9,               /* RX preamble code. Used in RX only. */
	1,               /* 0 to use standard SFD, 1 to use non-standard SFD. */
	DWT_BR_110K,     /* Data rate. */
	DWT_PHRMODE_STD, /* PHY header mode. */
	(1025 + 64 - 32) /* SFD timeout (preamble length + 1 + SFD length - PAC size). Used in RX only. */
};

void dwt_onepos_init(uint8_t led_indicators)
{
	openspi();
	if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
	{
		printf("INIT FAILED");
		while (1)
		{ };
	}
	fast_spi();
	
	/* Configure DW1000. See NOTE 6 below. */
	dwt_configure(&config);
	
	if (led_indicators)
	{
		dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
	}
	
}

//uint8_t dwt_parse_message(uint8_t )

uint8_t dwt_string_counter(char * str)
{
	uint8_t counter=0;
	while (*str!=0)
	{
		str++;
		counter++;
	}
	return counter + 2; //Plus 2 CRC bytes
}

void dwt_show_sys_info(void)
{
	uint32_t dev_id;
	uint32_t dev_part_id;
	uint8_t dev_eui[EUI_64_LEN];
	uint8_t dev_panid[PANADR_LEN];
	uint32_t sysconf;
	uint8_t tx_fctrl[TX_FCTRL_LEN];
	uint32_t sys_ctrl;
	uint32_t sys_mask;
	uint8_t sys_status[SYS_STATUS_LEN];
	uint32_t rx_finfo;
	uint8_t rx_fqual[RX_FQUAL_LEN];
	uint32_t chan_ctrl;
	
	dev_id =dwt_readdevid();
	dev_part_id = dwt_getpartid();
	dwt_geteui(dev_eui);
	dwt_readfromdevice(PANADR_ID, 0, PANADR_LEN, dev_eui);
	sysconf = dwt_read32bitreg(SYS_CFG_ID);
	dwt_readfromdevice(TX_FCTRL_ID,0,TX_FCTRL_LEN,tx_fctrl);
	sys_ctrl = dwt_read32bitreg(SYS_CTRL_ID);
	sys_mask = dwt_read32bitreg(SYS_MASK_ID);
	dwt_readfromdevice(SYS_STATUS_ID,0,SYS_STATUS_LEN,sys_status);
	rx_finfo = dwt_read32bitreg(RX_FINFO_ID);
	dwt_readfromdevice(RX_FQUAL_ID,0,RX_FQUAL_LEN,rx_fqual);
	chan_ctrl = dwt_read32bitreg(CHAN_CTRL_ID);
	
	printf("\n------------------------------------------\n");
	printf("DWM1000 INFO\n");
	printf("Device ID  : %04X %04X\n", (uint16_t)(dev_id>>16),(uint16_t)dev_id);
	printf("Part ID    : %04X %04X\n", (uint16_t)(dev_part_id>>16),(uint16_t)dev_part_id);
	printf("EUI        : %02X %02X %02X %02X %02X %02X %02X %02X\n", dev_eui[7], dev_eui[6],dev_eui[5],dev_eui[4],dev_eui[3],dev_eui[2],dev_eui[1],dev_eui[0]);
	printf("PAN Address: %02X %02X %02X %02X\n",dev_panid[3], dev_panid[2], dev_panid[1], dev_panid[0]);
	printf("SYS_CFG    : %04X %04X\n", (uint16_t)(sysconf>>16),(uint16_t)sysconf);
	printf("TX_FCTRL   : %02X %02X %02X %02X %02X\n", tx_fctrl[4], tx_fctrl[3], tx_fctrl[2], tx_fctrl[1], tx_fctrl[0]);
	printf("SYS_CTRL   : %04X %04X\n", (uint16_t)(sys_ctrl>>16),(uint16_t)sys_ctrl);
	printf("SYS_MASK   : %04X %04X\n", (uint16_t)(sys_mask>>16),(uint16_t)sys_mask);
	printf("SYS_STATUS : %02X %02X %02X %02X %02X\n", sys_status[4], sys_status[3], sys_status[2], sys_status[1], sys_status[0]);
	printf("RX_FINFO   : %04X %04X\n", (uint16_t)(rx_finfo>>16),(uint16_t)rx_finfo);
	printf("RX_FQUAL   : %02X %02X %02X %02X %02X %02X %02X %02X\n", rx_fqual[7], rx_fqual[6],rx_fqual[5],rx_fqual[4],rx_fqual[3],rx_fqual[2],rx_fqual[1],rx_fqual[0]);
	printf("CHAN_CTRL  : %04X %04X\n", (uint16_t)(chan_ctrl>>16),(uint16_t)chan_ctrl);
	printf("\n------------------------------------------\n");
	
}


	/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn get_tx_timestamp_u64()
	 *
	 * @brief Get the TX time-stamp in a 64-bit variable.
	 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
	 *
	 * @param  none
	 *
	 * @return  64-bit value of the read time-stamp.
	 */
	static uint64_t get_tx_timestamp_u64(void)
	{
		uint8 ts_tab[5];
		uint64_t ts = 0;
		int i;
		dwt_readtxtimestamp(ts_tab);
		for (i = 4; i >= 0; i--)
		{
			ts <<= 8;
			ts |= ts_tab[i];
		}
		return ts;
	}

	/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn get_rx_timestamp_u64()
	 *
	 * @brief Get the RX time-stamp in a 64-bit variable.
	 *        /!\ This function assumes that length of time-stamps is 40 bits, for both TX and RX!
	 *
	 * @param  none
	 *
	 * @return  64-bit value of the read time-stamp.
	 */
	static uint64_t get_rx_timestamp_u64(void)
	{
		uint8 ts_tab[5];
		uint64_t ts = 0;
		int i;
		dwt_readrxtimestamp(ts_tab);
		for (i = 4; i >= 0; i--)
		{
			ts <<= 8;
			ts |= ts_tab[i];
		}
		return ts;
	}

	/*! ------------------------------------------------------------------------------------------------------------------
	 * @fn final_msg_set_ts()
	 *
	 * @brief Fill a given timestamp field in the final message with the given value. In the timestamp fields of the final
	 *        message, the least significant byte is at the lower address.
	 *
	 * @param  ts_field  pointer on the first byte of the timestamp field to fill
	 *         ts  timestamp value
	 *
	 * @return none
	 */
	static void final_msg_set_ts(uint8 *ts_field, uint64_t ts)
	{
		int i;
		for (i = 0; i < FINAL_MSG_TS_LEN; i++)
		{
			ts_field[i] = (uint8) ts;
			ts >>= 8;
		}
	}