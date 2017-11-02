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

/* Declaration of static functions. */
static uint64_t get_tx_timestamp_u64(void);
static uint64_t get_rx_timestamp_u64(void);
static void final_msg_set_ts(uint8 *ts_field, uint64_t ts);
static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts);

void rx_ok_cb(const dwt_cb_data_t *cb_data);
void rx_to_cb(const dwt_cb_data_t *cb_data);
void rx_err_cb(const dwt_cb_data_t *cb_data);
void tx_conf_cb(const dwt_cb_data_t *cb_data);

uint8_t msg_counter = 0;


void dwt_onepos_init(uint8_t led_indicators)
{
	openspi();
	if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
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
	
    dwt_setrxantennadelay(onepos_get_uwb_rx_antenna_delay());
    dwt_settxantennadelay(onepos_get_uwb_tx_antenna_delay());
	
	
	/* Register RX call-back. */
	dwt_setcallbacks(&tx_conf_cb, &rx_ok_cb, &rx_to_cb, &rx_err_cb);
	/* Enable wanted interrupts (TX confirmation, RX good frames, RX timeouts and RX errors). */
	dwt_setinterrupt(DWT_INT_TFRS | DWT_INT_RFCG | DWT_INT_RFTO | DWT_INT_RXPTO | DWT_INT_RPHE | DWT_INT_RFCE | DWT_INT_RFSL | DWT_INT_SFDT, 1);
	/* Set delay to turn reception on after transmission of the frame. See NOTE 2 below. */
	//dwt_setrxaftertxdelay(TX_TO_RX_DELAY_UUS);
	/* Set response frame timeout. */
	//dwt_setrxtimeout(RX_RESP_TO_UUS);
	
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

uint8_t dwt_send_msg_w_ack(char * msg, uint16_t dest_address)
{
	char msg_complete[strlen(msg) + 11 + 2];
	
	msg_complete[0] = 0x41; //MSG Sequence start
	msg_complete[1] = 0x88; //MSG sequence start
	msg_complete[2] = msg_counter; //Sequence number
	msg_complete[3] = 0x00; //PAN ID MSB
	msg_complete[4] = 0x00; //PAN ID LSB
	msg_complete[5] = (char)(dest_address >> 8); //Dest address MSB
	msg_complete[6] = (char)dest_address; //Dest address LSB
	msg_complete[7] = 0x0; //Source address MSB
	msg_complete[8] = (char)onepos_get_node_id(); //Source address LSB
	msg_complete[9] = DECA_STRING_MSG; //type of message code
	msg_complete[10] = strlen(msg);
	msg_complete[strlen(msg) + 11] = 0x00; //CRC
	msg_complete[strlen(msg) + 12] = 0x00; //CRC
	
	strncpy(&msg_complete[11], msg, strlen(msg));
	
	
	/* Reset the TX delay and event signalling mechanism ready to await the next event. */
	tx_delay_ms = -1; //global variable
	rx_flag = -1;
	
	dwt_writetxdata(sizeof(msg_complete), msg_complete, 0); /* Zero offset in TX buffer. */
	dwt_writetxfctrl(sizeof(msg_complete), 0, 0); /* Zero offset in TX buffer, no ranging. */

	/* Start transmission, indicating that a response is expected so that reception is enabled immediately after the frame is sent. */
	dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

	/* Wait for any RX event. */
	while (tx_delay_ms == -1)
	{
		if (ioport_get_pin_level(UWB_INTERRUPT))
		{
			dwt_isr();
		}
	}
	
	if (rx_flag == UWB_RX_ACK)
	{
		if (uwb_rx_message.seq_number == msg_counter)
		{
			rx_flag = UWB_RX_OK;
			/* Increment if the ack seq number is the same, increment */
			msg_counter++;
		}	
	}
	return rx_flag;
}

uint8_t dwt_receive_msg_w_ack(char * msg)
{
	rx_flag = -1;
	char ack_msg[10+2];
	
	/* Activate reception immediately. See NOTE 4 below. */
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
	dwt_setrxtimeout(0x0FFF); //Deactivate rx timeout

	while (rx_flag == -1)
	{
		if (ioport_get_pin_level(UWB_INTERRUPT))
		{
			dwt_isr();
		}
	}
	
	if (rx_flag == UWB_RX_OK)
	{
		ack_msg[0] = 0x41; //MSG Sequence start
		ack_msg[1] = 0x88; //MSG sequence start
		ack_msg[2] = uwb_rx_message.seq_number; //Sequence number
		ack_msg[3] = 0x00; //PAN ID MSB
		ack_msg[4] = 0x00; //PAN ID LSB
		ack_msg[5] = (char)(uwb_rx_message.source_address >> 8); //Dest address MSB
		ack_msg[6] = (char)(uwb_rx_message.source_address); //Dest address LSB
		ack_msg[7] = 0x0; //Source address MSB
		ack_msg[8] = (char)onepos_get_node_id(); //Source address LSB
		ack_msg[9] = DECA_ACK_MSG; //type of message code
		ack_msg[10] = 0x00; //CRC
		ack_msg[11] = 0x00; //CRC
		
		/* Write response frame data to DW1000 and prepare transmission. See NOTE 6 below.*/
		dwt_writetxdata(sizeof(ack_msg), ack_msg, 0); /* Zero offset in TX buffer. */
		dwt_writetxfctrl(sizeof(ack_msg), 0, 0); /* Zero offset in TX buffer, no ranging. */
		
		/* Send the response. */
		dwt_starttx(DWT_START_TX_IMMEDIATE);
		
		/* Poll DW1000 until TX frame sent event set. */
		while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
		{ 
			#ifdef LED_DBG
			led3_toogle();
			#endif
			};
		
		/* Clear TX frame sent event. */
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
		
		strcpy(msg,uwb_rx_message.msg);
	}
	
	return rx_flag;
}

uint8_t dwt_init_twr(uint16_t dest_address)
{
	rx_flag = -1;
	
	/* Buffer to store received response message.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	static uint8 rx_buffer[RX_BUF_LEN];
	
	/* Time-stamps of frames transmission/reception, expressed in device time units.
	* As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
	static uint64_t poll_tx_ts;
	static uint64_t resp_rx_ts;
	static uint64_t final_tx_ts;
	
	/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
	static uint32 status_reg = 0;
	
	uint8_t tx_poll_msg[10+2];
	uint8_t tx_final_msg[10+12+2];
	
	tx_poll_msg[0] = 0x41; //MSG Sequence start
	tx_poll_msg[1] = 0x88; //MSG sequence start
	tx_poll_msg[2] = msg_counter; //Sequence number
	tx_poll_msg[3] = 0x00; //PAN ID MSB
	tx_poll_msg[4] = 0x00; //PAN ID LSB
	tx_poll_msg[5] = (uint8_t)(dest_address >> 8); //Dest address MSB
	tx_poll_msg[6] = (uint8_t)(dest_address); //Dest address LSB
	tx_poll_msg[7] = 0x0; //Source address MSB
	tx_poll_msg[8] = (uint8_t)onepos_get_node_id(); //Source address LSB
	tx_poll_msg[9] = DECA_POLL_MSG; //type of message code
	tx_poll_msg[10] = 0x00; //CRC
	tx_poll_msg[11] = 0x00; //CRC
	
	tx_final_msg[0] = 0x41; //MSG Sequence start
	tx_final_msg[1] = 0x88; //MSG sequence start
	tx_final_msg[2] = msg_counter; //Sequence number
	tx_final_msg[3] = 0x00; //PAN ID MSB
	tx_final_msg[4] = 0x00; //PAN ID LSB
	tx_final_msg[5] = (uint8_t)(dest_address >> 8);; //Dest address MSB
	tx_final_msg[6] = (uint8_t)(dest_address); //Dest address LSB
	tx_final_msg[7] = 0x0; //Source address MSB
	tx_final_msg[8] = (uint8_t)onepos_get_node_id(); //Source address LSB
	tx_final_msg[9] = DECA_FINAL_MSG; //type of message code
	
	tx_final_msg[22] = 0x00; //CRC
	tx_final_msg[23] = 0x00; //CRC
	
	static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0, 0, 0, 5, 0, 1, DECA_RESP_MSG, 0x02, 0, 0, 0, 0};
	
	
	/* Set expected response's delay and timeout. See NOTE 4, 5 and 6 below.
		* As this example only handles one incoming frame with always the same delay and timeout, those values can be set here once for all. */
	dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
	dwt_setrxtimeout(20000); //RESP_RX_TIMEOUT_UUS
	dwt_setpreambledetecttimeout(50); //PRE_TIMEOUT

	/* Write frame data to DW1000 and prepare transmission. See NOTE 8 below. */
	dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0); /* Zero offset in TX buffer. */
	dwt_writetxfctrl(sizeof(tx_poll_msg), 0, 1); /* Zero offset in TX buffer, ranging. */

	/* Start transmission, indicating that a response is expected so that reception is enabled automatically after the frame is sent and the delay
		* set by dwt_setrxaftertxdelay() has elapsed. */
	dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

	//while (rx_flag == -1)
	//{
		//if (ioport_get_pin_level(UWB_INTERRUPT))
		//{
			//dwt_isr();
		//}
	//}
	//
	//if (rx_flag == UWB_RX_RESP)
	//{
		//msg_counter++;
		//
		/////* Clear good RX frame event and TX frame sent in the DW1000 status register. */
		//dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);
		//
		//uint32 final_tx_time;
		//int ret;
//
		///* Retrieve poll transmission and response reception timestamp. */
		//poll_tx_ts = get_tx_timestamp_u64();
		//resp_rx_ts = get_rx_timestamp_u64();
//
		///* Compute final message transmission time. See NOTE 10 below. */
		//final_tx_time = (resp_rx_ts + (RESP_RX_TO_FINAL_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
		//dwt_setdelayedtrxtime(final_tx_time);
//
		///* Final TX timestamp is the transmission time we programmed plus the TX antenna delay. */
		//final_tx_ts = (((uint64_t)(final_tx_time & 0xFFFFFFFEUL)) << 8) + (uint64_t)onepos_get_uwb_tx_antenna_delay();//;
		//
		//
		///* Write all timestamps in the final message. See NOTE 11 below. */
		//final_msg_set_ts(&tx_final_msg[FINAL_MSG_POLL_TX_TS_IDX], poll_tx_ts);
		//final_msg_set_ts(&tx_final_msg[FINAL_MSG_RESP_RX_TS_IDX], resp_rx_ts);
		//final_msg_set_ts(&tx_final_msg[FINAL_MSG_FINAL_TX_TS_IDX], final_tx_ts);
//
		///* Write and send final message. See NOTE 8 below. */
		//tx_final_msg[ALL_MSG_SN_IDX] = msg_counter;
		//dwt_writetxdata(sizeof(tx_final_msg), tx_final_msg, 0); /* Zero offset in TX buffer. */
		//dwt_writetxfctrl(sizeof(tx_final_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
		//ret = dwt_starttx(DWT_START_TX_DELAYED);
//
		///* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 12 below. */
		//if (ret == DWT_SUCCESS)
		//{
			///* Poll DW1000 until TX frame sent event set. See NOTE 9 below. */
			//while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
			//{led4_toogle(); };
//
			///* Clear TXFRS event. */
			//dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
//
			///* Increment frame sequence number after transmission of the final message (modulo 256). */
			//msg_counter++;
			//return UWB_RX_OK;
		//}
		//else
		//{
			//printf("dwt_starttx_failed");
		//}
	//}
	//else
	//{
		///* Clear RX error/timeout events in the DW1000 status register. */
		//dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);
//
		///* Reset RX to properly reinitialise LDE operation. */
		//dwt_rxreset();
	//}
	//
	//return UWB_RX_FAILED;
	
	
	/* We assume that the transmission is achieved correctly, poll for reception of a frame or error/timeout. See NOTE 9 below. */
	while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
	{ 
		#ifdef LED_DBG
		led1_toogle();
		#endif
	};

	/* Increment frame sequence number after transmission of the poll message (modulo 256). */
	msg_counter++;

	if (status_reg & SYS_STATUS_RXFCG)
	{
		#ifdef LED_DBG
		led2_toogle();
		#endif
		uint32 frame_len;

		/* Clear good RX frame event and TX frame sent in the DW1000 status register. */
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);

		/* A frame has been received, read it into the local buffer. */
		frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
		if (frame_len <= RX_BUF_LEN)
		{
			dwt_readrxdata(rx_buffer, frame_len, 0);
			#ifdef LED_DBG
			led3_toogle();
			#endif
		}

		/* Check that the frame is the expected response from the companion "DS TWR responder" example.
			* As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
		rx_buffer[ALL_MSG_SN_IDX] = 0;
		if (memcmp(rx_buffer, rx_resp_msg, 2) == 0)
		{
			uint32 final_tx_time;
			int ret;

			/* Retrieve poll transmission and response reception timestamp. */
			poll_tx_ts = get_tx_timestamp_u64();
			resp_rx_ts = get_rx_timestamp_u64();

			/* Compute final message transmission time. See NOTE 10 below. */
			final_tx_time = (resp_rx_ts + (RESP_RX_TO_FINAL_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
			dwt_setdelayedtrxtime(final_tx_time);

			/* Final TX timestamp is the transmission time we programmed plus the TX antenna delay. */
			final_tx_ts = (((uint64_t)(final_tx_time & 0xFFFFFFFEUL)) << 8) + (uint64_t)onepos_get_uwb_tx_antenna_delay();//;
			
			
			/* Write all timestamps in the final message. See NOTE 11 below. */
			final_msg_set_ts(&tx_final_msg[FINAL_MSG_POLL_TX_TS_IDX], poll_tx_ts);
			final_msg_set_ts(&tx_final_msg[FINAL_MSG_RESP_RX_TS_IDX], resp_rx_ts);
			final_msg_set_ts(&tx_final_msg[FINAL_MSG_FINAL_TX_TS_IDX], final_tx_ts);

			/* Write and send final message. See NOTE 8 below. */
			tx_final_msg[ALL_MSG_SN_IDX] = msg_counter;
			dwt_writetxdata(sizeof(tx_final_msg), tx_final_msg, 0); /* Zero offset in TX buffer. */
			dwt_writetxfctrl(sizeof(tx_final_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
			ret = dwt_starttx(DWT_START_TX_DELAYED);

			/* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 12 below. */
			if (ret == DWT_SUCCESS)
			{
				/* Poll DW1000 until TX frame sent event set. See NOTE 9 below. */
				while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
				{
					#ifdef LED_DBG
					led4_toogle();
					#endif
					 };

				/* Clear TXFRS event. */
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

				/* Increment frame sequence number after transmission of the final message (modulo 256). */
				msg_counter++;
				return UWB_RX_OK;
			}
		}
	}
	else
	{
		/* Clear RX error/timeout events in the DW1000 status register. */
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

		/* Reset RX to properly reinitialise LDE operation. */
		dwt_rxreset();
	}

	return UWB_RX_FAILED;

}

uint8_t dwt_resp_twr(uint16_t * final_distance)
{
	rx_flag = -1;
	uint32 status_reg = 0;

	/* Timestamps of frames transmission/reception.
	 * As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
	uint64_t poll_rx_ts;
	uint64_t resp_tx_ts;
	uint64_t final_rx_ts;

	/* Hold copies of computed time of flight and distance here for reference so that it can be examined at a debug breakpoint. */
	double tof;
	double distance;
	
	uint8_t tx_resp_msg[10+2+2];
	tx_resp_msg[0] = 0x41; //MSG Sequence start
	tx_resp_msg[1] = 0x88; //MSG sequence start
	tx_resp_msg[2] = msg_counter; //Sequence number
	tx_resp_msg[3] = 0x00; //PAN ID MSB
	tx_resp_msg[4] = 0x00; //PAN ID LSB
	tx_resp_msg[5] = (uint8_t)(uwb_rx_message.source_address >> 8);; //Dest address MSB
	tx_resp_msg[6] = (uint8_t)(uwb_rx_message.source_address); //Dest address LSB
	tx_resp_msg[7] = 0x0; //Source address MSB
	tx_resp_msg[8] = (uint8_t)onepos_get_node_id(); //Source address LSB
	tx_resp_msg[9] = DECA_RESP_MSG; //type of message code
	tx_resp_msg[10] = 0x00;
	tx_resp_msg[11] = 0x00;
	tx_resp_msg[12] = 0x00; //CRC
	tx_resp_msg[13] = 0x00; //CRC
	
	/* Set preamble timeout for expected frames. See NOTE 6 below. */
	//dwt_setpreambledetecttimeout(PRE_TIMEOUT);

	/* Clear reception timeout to start next ranging process. */
	dwt_setrxtimeout(0xffff);
	//dwt_setrxtimeout(0xff);

	/* Activate reception immediately. */
	dwt_rxenable(DWT_START_RX_IMMEDIATE);

	while (rx_flag == -1)
	{
		if (ioport_get_pin_level(UWB_INTERRUPT))
		{
			dwt_isr();
		}
	}
	
	if (rx_flag == UWB_RX_POLL)
	{
		uint32 resp_tx_time;
		int ret;

		/* Retrieve poll reception timestamp. */
		poll_rx_ts = get_rx_timestamp_u64();

		/* Set send time for response. See NOTE 9 below. */
		resp_tx_time = (uint64_t)(poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
		dwt_setdelayedtrxtime(resp_tx_time);
		
		/* Set expected delay and timeout for final message reception. See NOTE 4 and 5 below. */
		dwt_setrxaftertxdelay(RESP_TX_TO_FINAL_RX_DLY_UUS);
		dwt_setrxtimeout(FINAL_RX_TIMEOUT_UUS);

		/* Write and send the response message. See NOTE 10 below.*/
		tx_resp_msg[ALL_MSG_SN_IDX] = msg_counter;
		dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0); /* Zero offset in TX buffer. */
		dwt_writetxfctrl(sizeof(tx_resp_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
		
		ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
		
		/* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 11 below. */
		if (ret == DWT_ERROR)
		{
			printf("DWT_ERROR\n");
			return UWB_RX_FAILED;
		}
		
		rx_flag = -1;
		
		while (rx_flag == -1)
		{
			if (ioport_get_pin_level(UWB_INTERRUPT))
			{
				dwt_isr();
			}
		}
		
		if (rx_flag == UWB_RX_FINAL)
		{
			uint32 poll_tx_ts, resp_rx_ts, final_tx_ts;
			uint32 poll_rx_ts_32, resp_tx_ts_32, final_rx_ts_32;
			double Ra, Rb, Da, Db;
			int64_t tof_dtu;

			/* Retrieve response transmission and final reception timestamps. */
			resp_tx_ts = get_tx_timestamp_u64();
			final_rx_ts = get_rx_timestamp_u64();

			/* Get timestamps embedded in the final message. */
			final_msg_get_ts(&rx_buffer[FINAL_MSG_POLL_TX_TS_IDX], &poll_tx_ts);
			final_msg_get_ts(&rx_buffer[FINAL_MSG_RESP_RX_TS_IDX], &resp_rx_ts);
			final_msg_get_ts(&rx_buffer[FINAL_MSG_FINAL_TX_TS_IDX], &final_tx_ts);

			/* Compute time of flight. 32-bit subtractions give correct answers even if clock has wrapped. See NOTE 12 below. */
			poll_rx_ts_32 = (uint32)poll_rx_ts;
			resp_tx_ts_32 = (uint32)resp_tx_ts;
			final_rx_ts_32 = (uint32)final_rx_ts;
			Ra = (double)(resp_rx_ts - poll_tx_ts);
			Rb = (double)(final_rx_ts_32 - resp_tx_ts_32);
			Da = (double)(final_tx_ts - resp_rx_ts);
			Db = (double)(resp_tx_ts_32 - poll_rx_ts_32);
			tof_dtu = (int64_t)((Ra * Rb - Da * Db) / (Ra + Rb + Da + Db));

			tof = tof_dtu * DWT_TIME_UNITS;
			distance = tof * SPEED_OF_LIGHT;

			#ifdef DBG
			/* Display computed distance on LCD. */
			printf("DIST: %d mm\n", (uint16_t)(distance*100));
			printf("poll_tx_ts: %lX%lX, resp_rx_ts: %lX%lX, final_tx_ts: %lX%lX\n",(uint32_t)(poll_tx_ts >> 32),(uint32_t)(poll_tx_ts), (uint32_t)(resp_rx_ts>>32), (uint32_t)(resp_rx_ts) ,(uint32_t)(final_tx_ts>>32), (uint32_t)(final_tx_ts));
			printf("poll_rx_ts: %lX, resp_tx_ts: %lX, final_rx_ts: %lX\n\n",poll_rx_ts_32,resp_tx_ts_32,final_rx_ts_32);
			#endif 
			
			*final_distance = (uint16_t)(distance*100);
			return UWB_RX_OK;
			
		}
		else
		{
			/* Clear RX error/timeout events in the DW1000 status register. */
			dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

			/* Reset RX to properly reinitialise LDE operation. */
			dwt_rxreset();
		}

	}
	else
	{
		/* Clear RX error/timeout events in the DW1000 status register. */
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

		/* Reset RX to properly reinitialise LDE operation. */
		dwt_rxreset();
	}
	
	return UWB_RX_FAILED;
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

/*! ------------------------------------------------------------------------------------------------------------------
	* @fn final_msg_get_ts()
	*
	* @brief Read a given timestamp value from the final message. In the timestamp fields of the final message, the least
	*        significant byte is at the lower address.
	*
	* @param  ts_field  pointer on the first byte of the timestamp field to read
	*         ts  timestamp value
	*
	* @return none
	*/
static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts)
{
	int i;
	*ts = 0;
	for (i = 0; i < FINAL_MSG_TS_LEN; i++)
	{
		*ts += (uint64_t)(ts_field[i]) << (i * 8);
	}
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_ok_cb()
 *
 * @brief Callback to process RX good frame events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_ok_cb(const dwt_cb_data_t *cb_data)
{
	#ifdef DBG
	printf("RX OK! :D\n");
	#endif
	
    int i;

    /* Clear local RX buffer to avoid having leftovers from previous receptions. This is not necessary but is included here to aid reading the RX
     * buffer. */
    for (i = 0 ; i < FRAME_LEN_MAX; i++ )
    {
        rx_buffer[i] = 0;
    }

    /* A frame has been received, copy it to our local buffer. */
    if (cb_data->datalength <= FRAME_LEN_MAX)
    {
        dwt_readrxdata(rx_buffer, cb_data->datalength, 0);
    }
	
	//check destination and if message is valid
	if (rx_buffer[0] == 0x41 && rx_buffer[1] == 0x88)
	{
		
		uwb_rx_message.seq_number = rx_buffer[2];
		uwb_rx_message.pan_id =  (rx_buffer[3] << 8) | rx_buffer[4];
		uwb_rx_message.dest_address = (rx_buffer[5] << 8) | rx_buffer[6];
		uwb_rx_message.source_address = (rx_buffer[7] << 8) | rx_buffer[8];
		uwb_rx_message.msg_len = 0;
		
		
		if (uwb_rx_message.dest_address == onepos_get_node_id())
		{
			#ifdef DBG
			printf("valid message received.\n Sequence number = %d\n PanID= %d\n destination = %d\n source= %d\n msg type = %X\n\n",uwb_rx_message.seq_number, uwb_rx_message.pan_id, uwb_rx_message.dest_address, uwb_rx_message.source_address,rx_buffer[9]);
			#endif
			if (rx_buffer[9] == DECA_STRING_MSG)
			{
				uwb_rx_message.msg_len = rx_buffer[10];
				strncpy(uwb_rx_message.msg, &rx_buffer[11], uwb_rx_message.msg_len);
				#ifdef DBG
				printf("message: %s",uwb_rx_message.msg);
				#endif
				rx_flag = UWB_RX_OK;
			}
			else if (rx_buffer[9] == DECA_ACK_MSG)
			{
				#ifdef DBG
				printf("ACK received\n");
				#endif
				rx_flag = UWB_RX_ACK;
			}
			else if (rx_buffer[9] == DECA_POLL_MSG)
			{
				#ifdef DBG
				printf("POLL MSG received\n");
				#endif
				rx_flag = UWB_RX_POLL;
			}
			else if (rx_buffer[9] == DECA_FINAL_MSG)
			{
				#ifdef DBG
				printf("FINAL MSG received\n");
				#endif
				rx_flag = UWB_RX_FINAL;
			}
			else if(rx_buffer[9] == DECA_RESP_MSG)
			{
				#ifdef DBG
				printf("RESPOND MSG received\n");
				#endif
				rx_flag = UWB_RX_RESP;
			}
		}
		else if (uwb_rx_message.dest_address == 0xFF)
		{
			#ifdef DBG
			printf("valid message received from bradcast.\n Sequence number = %d\n PanID= %d\n destination = %d\n source= %d\n msg type = %X\n\n\n",uwb_rx_message.seq_number, uwb_rx_message.pan_id, uwb_rx_message.dest_address, uwb_rx_message.source_address,rx_buffer[9]);
			#endif
			rx_flag = UWB_RX_OK_BRDCST;
		}
		else
		{
			#ifdef DBG
			printf("That message was not for you\n");
			#endif
			rx_flag = UWB_RX_OK_NOTFORME;
		}
		
	}
	else
	{
		#ifdef DBG
		printf("Not a valid message\n");
		#endif
	}

    /* Set corresponding inter-frame delay. */
    tx_delay_ms = DFLT_TX_DELAY_MS;
	
	

    /* TESTING BREAKPOINT LOCATION #1 */
	#ifdef LED_DBG
	led1_toogle();
	#endif
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_to_cb()
 *
 * @brief Callback to process RX timeout events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_to_cb(const dwt_cb_data_t *cb_data)
{
	//printf("RX Timeout :(\n");
    /* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_TO_TX_DELAY_MS;

	rx_flag = UWB_RX_TO;

    /* TESTING BREAKPOINT LOCATION #2 */
	#ifdef LED_DBG
	led2_toogle();
	#endif
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_err_cb()
 *
 * @brief Callback to process RX error events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_err_cb(const dwt_cb_data_t *cb_data)
{
	#ifdef DBG
	printf("rx_err_cb\n");
    #endif
	/* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_ERR_TX_DELAY_MS;
	rx_flag = UWB_RX_FAILED;
    /* TESTING BREAKPOINT LOCATION #3 */
	#ifdef LED_DBG
	led3_toogle();
	#endif
}

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn tx_conf_cb()
 *
 * @brief Callback to process TX confirmation events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void tx_conf_cb(const dwt_cb_data_t *cb_data)
{
    /* This callback has been defined so that a breakpoint can be put here to check it is correctly called but there is actually nothing specific to
     * do on transmission confirmation in this example. Typically, we could activate reception for the response here but this is automatically handled
     * by DW1000 using DWT_RESPONSE_EXPECTED parameter when calling dwt_starttx().
     * An actual application that would not need this callback could simply not define it and set the corresponding field to NULL when calling
     * dwt_setcallbacks(). The ISR will not call it which will allow to save some interrupt processing time. */

    /* TESTING BREAKPOINT LOCATION #4 */
	//printf("tx_conf_cb\n");
	#ifdef LED_DBG
	led4_toogle();
	#endif
}



void dwt_node1_calibration(uint16_t node2_address, uint16_t node3_address)
{
	uint16 dist;
	char msg[10];
	led1(ON);
	led2(OFF);
	led3(OFF);
	led4(OFF);
	uint8_t i = 0;
	uint8_t status = -1;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node2_address); //INIT D12
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	
	led1(OFF);
	led2(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node3_address); //INIT D13
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	led1(ON);
	led2(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_resp_twr(&dist); //RESP D21
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d21,%d\n",i,dist);
		}
	}
	led1(OFF);
	led2(OFF);
	led3(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_resp_twr(&dist); //RESP D31
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d31,%d\n",i,dist);
		}
	}
	led1(ON);
	led2(OFF);
	led3(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_receive_msg_w_ack(msg); //receive D12
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d12,%s\n",i,msg);
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(ON);
	led3(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_receive_msg_w_ack(msg); //receive D32
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d32,%s\n",i,msg);
			delay_ms(2);
		}
	}
	led1(ON);
	led2(ON);
	led3(ON);
	i=0;
	
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_receive_msg_w_ack(msg); //receive D13
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d13,%s\n",i,msg);
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(OFF);
	led3(OFF);
	led4(ON);
	i=0;
	
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_receive_msg_w_ack(msg); //receive D23
		if (status == UWB_RX_OK)
		{
			i++;
			printf("%d,d23,%s\n",i,msg);
			delay_ms(2);
		}
	}
	led1(ON);
	led2(OFF);
	led3(OFF);
	led4(ON);
	i=0;
	for(;;)
	{
		led1_toogle();
		led2_toogle();
		led3_toogle();
		led4_toogle();
		delay_ms(500);
	}
	
	
}

void dwt_node2_calibration(uint16_t node1_address, uint16_t node3_address)
{
	uint16_t dist;
	uint16 dist12[CALIBRATION_ITERATIONS];
	uint16 dist32[CALIBRATION_ITERATIONS];
	char msg[10];
	led1(ON);
	led2(OFF);
	led3(OFF);
	led4(OFF);
	uint8_t i = 0;
	uint8_t status = -1;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_resp_twr(&dist); //RESP D12
		if (status == UWB_RX_OK)
		{
			dist12[i] = dist;
			i++;
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(ON);
	i=0;
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);

	
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node1_address); //INIT D21
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	led1(ON);
	led2(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node3_address); //INIT D23
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(OFF);
	led3(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_resp_twr(&dist); //RESP D32
		if (status == UWB_RX_OK)
		{
			dist32[i] = dist;
			i++;
			delay_ms(2);
		}
	}
	led1(ON);
	led2(OFF);
	led3(ON);
	i=0;
	sprintf(msg,"%d",dist12[i]);
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_send_msg_w_ack(msg,node1_address); //send D12
		if (status == UWB_RX_OK)
		{
			i++;
			sprintf(msg,"%d",dist12[i]);
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(ON);
	led3(ON);
	i=0;
	sprintf(msg,"%d",dist32[i]);
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_send_msg_w_ack(msg,node1_address); //send D32
		if (status == UWB_RX_OK)
		{
			i++;
			sprintf(msg,"%d",dist32[i]);
			delay_ms(2);
		}
	}
	
	for(;;)
	{
		led1_toogle();
		led2_toogle();
		led3_toogle();
		led4_toogle();
		delay_ms(500);
	}
}

void dwt_node3_calibration(uint16_t node1_address, uint16_t node2_address)
{
	uint16_t dist;
	uint16 dist13[CALIBRATION_ITERATIONS];
	uint16 dist23[CALIBRATION_ITERATIONS];
	char msg[10];
	led1(ON);
	led2(OFF);
	led3(OFF);
	led4(OFF);
	uint8_t i = 0;
	uint8_t status = -1;
	
	while( i < CALIBRATION_ITERATIONS )
	{
		status = dwt_resp_twr(&dist); //RESP D13
		if (status == UWB_RX_OK)
		{
			dist13[i] = dist;
			i++;
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_resp_twr(&dist); //RESP D23
		if (status == UWB_RX_OK)
		{
			dist23[i] = dist;
			i++;
			delay_ms(2);
		}
	}
	led1(ON);
	led2(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node1_address); //INIT D31
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(OFF);
	led3(ON);
	i=0;
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_init_twr(node2_address); //INIT D32
		if (status == UWB_RX_OK)
		{
			i++;
			delay_ms(2);
		}
	}
	led1(ON);
	led2(OFF);
	led3(ON);
	i=0;
	sprintf(msg,"%d",dist13[i]);
	
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_send_msg_w_ack(msg,node1_address); //send D13
		if (status == UWB_RX_OK)
		{
			i++;
			sprintf(msg,"%d",dist13[i]);
			delay_ms(2);
		}
	}
	led1(OFF);
	led2(ON);
	led3(ON);
	i=0;
	sprintf(msg,"%d",dist23[i]);
	while(i<CALIBRATION_ITERATIONS)
	{
		status = dwt_send_msg_w_ack(msg,node1_address); //send D23
		if (status == UWB_RX_OK)
		{
			i++;
			sprintf(msg,"%d",dist23[i]);
			delay_ms(2);
		}
	}
	
	for(;;)
	{
		led1_toogle();
		led2_toogle();
		led3_toogle();
		led4_toogle();
		delay_ms(500);
	}
}