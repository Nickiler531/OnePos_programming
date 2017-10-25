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

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_ok_cb()
 *
 * @brief Callback to process RX good frame events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_ok_cb(const dwt_cb_data_t *cb_data);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_to_cb()
 *
 * @brief Callback to process RX timeout events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_to_cb(const dwt_cb_data_t *cb_data);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn rx_err_cb()
 *
 * @brief Callback to process RX error events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void rx_err_cb(const dwt_cb_data_t *cb_data);

/*! ------------------------------------------------------------------------------------------------------------------
 * @fn tx_conf_cb()
 *
 * @brief Callback to process TX confirmation events
 *
 * @param  cb_data  callback data
 *
 * @return  none
 */
void tx_conf_cb(const dwt_cb_data_t *cb_data);


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
	
    dwt_setrxantennadelay(onepos_get_uwb_rx_antenna_delay());
    dwt_settxantennadelay(onepos_get_uwb_tx_antenna_delay());

    /* Set expected response's delay and timeout. See NOTE 4, 5 and 6 below.
     * As this example only handles one incoming frame with always the same delay and timeout, those values can be set here once for all. */
    //dwt_setrxaftertxdelay(POLL_TX_TO_RESP_RX_DLY_UUS);
    //dwt_setrxtimeout(RESP_RX_TIMEOUT_UUS);
    //dwt_setpreambledetecttimeout(PRE_TIMEOUT);
	
	
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
	static uint8_t msg_counter = 0;
	
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


uint8_t dwt_receive_msg_w_ack(void)
{
	rx_flag = -1;
	char ack_msg[10+2];
	
	/* Activate reception immediately. See NOTE 4 below. */
    dwt_rxenable(DWT_START_RX_IMMEDIATE);
	dwt_setrxtimeout(0); //Deactivate rx timeout

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
		{ led3_toogle(); };
		
		/* Clear TX frame sent event. */
		dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
	}
	
	return rx_flag;

    ///* Poll until a frame is properly received or an error occurs. See NOTE 5 below.
        //* STATUS register is 5 bytes long but, as the events we are looking at are in the lower bytes of the register, we can use this simplest API
        //* function to access it. */
    //while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
    //{ led1_toogle(); };
//
    //if (status_reg & SYS_STATUS_RXFCG)
    //{
        ///* A frame has been received, read it into the local buffer. */
        //frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
        //if (frame_len <= FRAME_LEN_MAX)
        //{
            //dwt_readrxdata(rx_buffer, frame_len, 0);
			//led2_toogle();
			//printf("%s\n",rx_buffer);
        //}
//
        ///* TESTING BREAKPOINT LOCATION #1 */
//
        ///* Clear good RX frame event in the DW1000 status register. */
        //dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
//
        ///* Validate the frame is the one expected as sent by "TX then wait for a response" example. */
        //if ((frame_len == 14) && (rx_buffer[0] == 0xC5) && (rx_buffer[10] == 0x43) && (rx_buffer[11] == 0x2))
        //{
            //int i;
//
            ///* Copy source address of blink in response destination address. */
            //for (i = 0; i < 8; i++)
            //{
                //tx_msg[DATA_FRAME_DEST_IDX + i] = rx_buffer[BLINK_FRAME_SRC_IDX + i];
            //}
//
            ///* Write response frame data to DW1000 and prepare transmission. See NOTE 6 below.*/
            //dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
            //dwt_writetxfctrl(sizeof(tx_msg), 0, 0); /* Zero offset in TX buffer, no ranging. */
//
            ///* Send the response. */
            //dwt_starttx(DWT_START_TX_IMMEDIATE);
//
            ///* Poll DW1000 until TX frame sent event set. */
            //while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
            //{ led3_toogle(); };
//
            ///* Clear TX frame sent event. */
            //dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);
//
            ///* Increment the data frame sequence number (modulo 256). */
            //tx_msg[DATA_FRAME_SN_IDX]++;
        //}
    //}
    //else
    //{
        ///* Clear RX error events in the DW1000 status register. */
        //dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
    //}
	
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
	printf("RX OK! :D\n");
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
		
		if (rx_buffer[9] == DECA_STRING_MSG)
		{
			uwb_rx_message.msg_len = rx_buffer[10];
			strncpy(uwb_rx_message.msg, &rx_buffer[11], uwb_rx_message.msg_len);
			
			if (uwb_rx_message.dest_address == onepos_get_node_id())
			{
				printf("valid message received.\n Sequence number = %d\n PanID= %d\n destination = %d\n source= %d\n msg lenght = %d\n message = %s\n\n",uwb_rx_message.seq_number, uwb_rx_message.pan_id, uwb_rx_message.dest_address, uwb_rx_message.source_address,uwb_rx_message.msg_len ,uwb_rx_message.msg);
				rx_flag = UWB_RX_OK;
			}
			else if (uwb_rx_message.dest_address == 0xFF)
			{
				printf("valid message received from bradcast.\n Sequence number = %d\n PanID= %d\n destination = %d\n source= %d\n msg lenght = %d\n message = %s\n\n",uwb_rx_message.seq_number, uwb_rx_message.pan_id, uwb_rx_message.dest_address, uwb_rx_message.source_address,uwb_rx_message.msg_len ,uwb_rx_message.msg);
				rx_flag = UWB_RX_OK_BRDCST;
			}
			else
			{
				printf("That message was not for you\n");
				rx_flag = UWB_RX_OK_NOTFORME;
			}
		}
		else if (rx_buffer[9] == DECA_ACK_MSG)
		{
			printf("ACK received\n");
			rx_flag = UWB_RX_ACK;
		}
		
		
	}
	else
	{
		printf("Not a valid message\n");
	}

    /* Set corresponding inter-frame delay. */
    tx_delay_ms = DFLT_TX_DELAY_MS;
	
	

    /* TESTING BREAKPOINT LOCATION #1 */
	led1_toogle();
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
	printf("RX Timeout :(\n");
    /* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_TO_TX_DELAY_MS;

	rx_flag = UWB_RX_TO;

    /* TESTING BREAKPOINT LOCATION #2 */
	led2_toogle();
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
	printf("rx_err_cb\n");
    /* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_ERR_TX_DELAY_MS;
	rx_flag = UWB_RX_FAILED;
    /* TESTING BREAKPOINT LOCATION #3 */
	led3_toogle();
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
	led4_toogle();
}
