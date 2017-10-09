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
#define USART_BLE                        &USARTD0
#define USART_SERIAL_BAUDRATE            115200
#define USART_SERIAL_CHAR_LENGTH         USART_CHSIZE_8BIT_gc
#define USART_SERIAL_PARITY              USART_PMODE_DISABLED_gc
#define USART_SERIAL_STOP_BIT            false

//#define DWT_SIMPLE_RX
//#define DWT_SIMPLE_TX
//#define DWT_DS_TWR_INIT
#define DWT_DS_TWR_RESP
//#define REST

//#define DWT_TX_WAIT_RESP_INTERRUPTS
//#define DWT_RX_SEND_RESPOND

void rs485_send(char a)
{
	
}

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

#ifdef DWT_SIMPLE_TX

	/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
	static dwt_config_t config = {
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

	/* The frame sent in this example is an 802.15.4e standard blink. It is a 12-byte frame composed of the following fields:
	 *     - byte 0: frame type (0xC5 for a blink).
	 *     - byte 1: sequence number, incremented for each new frame.
	 *     - byte 2 -> 9: device ID, see NOTE 1 below.
	 *     - byte 10/11: frame check-sum, automatically set by DW1000.  */
	static uint8 tx_msg[] = {0xC5, 0, 'D', 'E', 'C', 'A', 'W', 'A', 'V', 'E', 0, 0};
	/* Index to access to sequence number of the blink frame in the tx_msg array. */
	#define BLINK_FRAME_SN_IDX 1

	/* Inter-frame delay period, in milliseconds. */
	#define TX_DELAY_MS 1000

	void dwt_send_example(void)
	{
		if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
		{
			printf("INIT FAILED");
			while (1)
			{ };
		}
		fast_spi();

		/* Configure DW1000. See NOTE 3 below. */
		dwt_configure(&config);
		dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
		
		/* Loop forever sending frames periodically. */
		while(1)
		{
			led1_toogle();
			/* Write frame data to DW1000 and prepare transmission. See NOTE 4 below.*/
			dwt_writetxdata(dwt_string_counter("BLABLABLABLA123456789!@#$%^&*()"), "BLABLABLABLA123456789!@#$%^&*()", 0); /* Zero offset in TX buffer. */
			dwt_writetxfctrl(dwt_string_counter("BLABLABLABLA123456789!@#$%^&*()"), 0, 0); /* Zero offset in TX buffer, no ranging. */

			/* Start transmission. */
			dwt_starttx(DWT_START_TX_IMMEDIATE);

			/* Poll DW1000 until TX frame sent event set. See NOTE 5 below.
			 * STATUS register is 5 bytes long but, as the event we are looking at is in the first byte of the register, we can use this simplest API
			 * function to access it.*/
			while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
			{ };

			/* Clear TX frame sent event. */
			dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

			/* Execute a delay between transmissions. */
			delay_ms(TX_DELAY_MS);

			/* Increment the blink frame sequence number (modulo 256). */
			tx_msg[BLINK_FRAME_SN_IDX]++;
		
			dwt_show_sys_info();
		}
	}

#endif // end of #ifdef DWT_SIMPLE_SEND

#ifdef DWT_SIMPLE_RX

/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
static dwt_config_t config = {
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

/* Buffer to store received frame. See NOTE 1 below. */
#define FRAME_LEN_MAX 127
static uint8 rx_buffer[FRAME_LEN_MAX];

/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
static uint32 status_reg = 0;

/* Hold copy of frame length of frame received (if good) so that it can be examined at a debug breakpoint. */
static uint16 frame_len = 0;

void dwt_receive_example(void)
{
	if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
	{
		printf("INIT FAILED");
		while (1)
		{ };
	}
	fast_spi();

	/* Configure DW1000. See NOTE 3 below. */
	dwt_configure(&config);
	dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
	
	while (1)
    {
    
        int i;

        /* TESTING BREAKPOINT LOCATION #1 */

        /* Clear local RX buffer to avoid having leftovers from previous receptions  This is not necessary but is included here to aid reading
         * the RX buffer.
         * This is a good place to put a breakpoint. Here (after first time through the loop) the local status register will be set for last event
         * and if a good receive has happened the data buffer will have the data in it, and frame_len will be set to the length of the RX frame. */
        for (i = 0 ; i < FRAME_LEN_MAX; i++ )
        {
            rx_buffer[i] = 0;
        }

        /* Activate reception immediately. See NOTE 3 below. */
        dwt_rxenable(DWT_START_RX_IMMEDIATE);

        /* Poll until a frame is properly received or an error/timeout occurs. See NOTE 4 below.
         * STATUS register is 5 bytes long but, as the event we are looking at is in the first byte of the register, we can use this simplest API
         * function to access it. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { };

        if (status_reg & SYS_STATUS_RXFCG)
        {
            /* A frame has been received, copy it to our local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
            if (frame_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, frame_len, 0);
				printf("%s\n",rx_buffer);
            }

            /* Clear good RX frame event in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);
        }
        else
        {
            /* Clear RX error events in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
        }
		delay_ms(1000);
    }
}

#endif


#ifdef DWT_TX_WAIT_RESP_INTERRUPTS

/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
static dwt_config_t config = {
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

/* The frame sent in this example is a blink encoded as per the ISO/IEC 24730-62:2013 standard. It is a 14-byte frame composed of the following fields:
 *     - byte 0: frame control (0xC5 to indicate a multipurpose frame using 64-bit addressing).
 *     - byte 1: sequence number, incremented for each new frame.
 *     - byte 2 -> 9: device ID, see NOTE 1 below.
 *     - byte 10: encoding header (0x43 to indicate no extended ID, temperature, or battery status is carried in the message).
 *     - byte 11: EXT header (0x02 to indicate tag is listening for a response immediately after this message).
 *     - byte 12/13: frame check-sum, automatically set by DW1000. */
static uint8 tx_msg[] = {0xC5, 0, 'D', 'E', 'C', 'A', 'W', 'A', 'V', 'E', 0x43, 0x02, 0, 0};
/* Index to access the sequence number of the blink frame in the tx_msg array. */
#define BLINK_FRAME_SN_IDX 1

/* Delay from end of transmission to activation of reception, expressed in UWB microseconds (1 uus is 512/499.2 microseconds). See NOTE 2 below. */
#define TX_TO_RX_DELAY_UUS 60

/* Receive response timeout, expressed in UWB microseconds. See NOTE 3 below. */
#define RX_RESP_TO_UUS 5000

/* Default inter-frame delay period, in milliseconds. */
#define DFLT_TX_DELAY_MS 1000
/* Inter-frame delay period in case of RX timeout, in milliseconds.
 * In case of RX timeout, assume the receiver is not present and lower the rate of blink transmission. */
#define RX_TO_TX_DELAY_MS 3000
/* Inter-frame delay period in case of RX error, in milliseconds.
 * In case of RX error, assume the receiver is present but its response has not been received for any reason and retry blink transmission immediately. */
#define RX_ERR_TX_DELAY_MS 0
/* Current inter-frame delay period.
 * This global static variable is also used as the mechanism to signal events to the background main loop from the interrupt handler callbacks,
 * which set it to positive delay values. */
static int32 tx_delay_ms = -1;

/* Buffer to store received frame. See NOTE 4 below. */
#define FRAME_LEN_MAX 127
static uint8 rx_buffer[FRAME_LEN_MAX];

/* Declaration of static functions. */
static void rx_ok_cb(const dwt_cb_data_t *cb_data);
static void rx_to_cb(const dwt_cb_data_t *cb_data);
static void rx_err_cb(const dwt_cb_data_t *cb_data);
static void tx_conf_cb(const dwt_cb_data_t *cb_data);

void dwt_tx_wait_resp_interrupts(void)
{
	if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
	{
		printf("INIT FAILED");
		while (1)
		{ };
	}
	fast_spi();
	
	/* Configure DW1000. See NOTE 6 below. */
	dwt_configure(&config);

	dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);

	/* Register RX call-back. */
	dwt_setcallbacks(&tx_conf_cb, &rx_ok_cb, &rx_to_cb, &rx_err_cb);

	/* Enable wanted interrupts (TX confirmation, RX good frames, RX timeouts and RX errors). */
	dwt_setinterrupt(DWT_INT_TFRS | DWT_INT_RFCG | DWT_INT_RFTO | DWT_INT_RXPTO | DWT_INT_RPHE | DWT_INT_RFCE | DWT_INT_RFSL | DWT_INT_SFDT, 1);

	/* Set delay to turn reception on after transmission of the frame. See NOTE 2 below. */
	dwt_setrxaftertxdelay(TX_TO_RX_DELAY_UUS);

	/* Set response frame timeout. */
	dwt_setrxtimeout(RX_RESP_TO_UUS);

	/* Loop forever sending and receiving frames periodically. */
	while (1)
	{
		/* Write frame data to DW1000 and prepare transmission. See NOTE 7 below. */
		dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
		dwt_writetxfctrl(sizeof(tx_msg), 0, 0); /* Zero offset in TX buffer, no ranging. */

		/* Start transmission, indicating that a response is expected so that reception is enabled immediately after the frame is sent. */
		dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

		/* Wait for any RX event. */
		while (tx_delay_ms == -1)
		{
			dwt_isr();	
		}

		/* Execute the defined delay before next transmission. */
		if (tx_delay_ms > 0)
		{
			delay_ms(tx_delay_ms);
		}

		/* Increment the blink frame sequence number (modulo 256). */
		tx_msg[BLINK_FRAME_SN_IDX]++;

		/* Reset the TX delay and event signalling mechanism ready to await the next event. */
		tx_delay_ms = -1;
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
static void rx_ok_cb(const dwt_cb_data_t *cb_data)
{
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
static void rx_to_cb(const dwt_cb_data_t *cb_data)
{
    /* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_TO_TX_DELAY_MS;

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
static void rx_err_cb(const dwt_cb_data_t *cb_data)
{
    /* Set corresponding inter-frame delay. */
    tx_delay_ms = RX_ERR_TX_DELAY_MS;

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
static void tx_conf_cb(const dwt_cb_data_t *cb_data)
{
    /* This callback has been defined so that a breakpoint can be put here to check it is correctly called but there is actually nothing specific to
     * do on transmission confirmation in this example. Typically, we could activate reception for the response here but this is automatically handled
     * by DW1000 using DWT_RESPONSE_EXPECTED parameter when calling dwt_starttx().
     * An actual application that would not need this callback could simply not define it and set the corresponding field to NULL when calling
     * dwt_setcallbacks(). The ISR will not call it which will allow to save some interrupt processing time. */

    /* TESTING BREAKPOINT LOCATION #4 */
	led4_toogle();
}

#endif //endif DWT_TX_WAIT_RESP_INTERRUPTS


#ifdef DWT_RX_SEND_RESPOND

/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
static dwt_config_t config = {
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

/* As "TX then wait for a response" example sends a blink message encoded as per the ISO/IEC 24730-62:2013 standard which includes a bit signalling
 * that a response is listened for, this example will respond with a valid frame (that will be ignored anyway) following the same standard. The
 * response is a 21-byte frame composed of the following fields:
 *     - byte 0/1: frame control (0x8C41 to indicate a data frame using 16-bit source addressing and 64-bit destination addressing).
 *     - byte 2: sequence number, incremented for each new frame.
 *     - byte 3/4: application ID (0x609A for data frames in this standard).
 *     - byte 5 -> 12: 64-bit destination address.
 *     - byte 13/14: 16-bit source address, hard coded in this example to keep it simple.
 *     - byte 15: function code (0x10 to indicate this is an activity control message).
 *     - byte 16: activity code (0x00 to indicate activity is finished).
 *     - byte 17/18: new tag blink rate.
 *     - byte 19/20: frame check-sum, automatically set by DW1000.  */
static uint8 tx_msg[] = {0x41, 0x8C, 0, 0x9A, 0x60, 0, 0, 0, 0, 0, 0, 0, 0, 'D', 'W', 0x10, 0x00, 0, 0, 0, 0};
/* Indexes to access to sequence number and destination address of the data frame in the tx_msg array. */
#define DATA_FRAME_SN_IDX 2
#define DATA_FRAME_DEST_IDX 5

/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

/* Buffer to store received frame. See NOTE 1 below. */
#define FRAME_LEN_MAX 127
static uint8 rx_buffer[FRAME_LEN_MAX];
/* Index to access to source address of the blink frame in the rx_buffer array. */
#define BLINK_FRAME_SRC_IDX 2

/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
static uint32 status_reg = 0;

/* Hold copy of frame length of frame received (if good) so that it can be examined at a debug breakpoint. */
static uint16 frame_len = 0;

void dwt_rx_send_resp(void)
{
	if (dwt_initialise(DWT_LOADNONE) == DWT_ERROR)
    {
        printf("INIT FAILED");
        while (1)
        { };
    }
    fast_spi();

    /* Configure DW1000. See NOTE 3 below. */
    dwt_configure(&config);
	
	dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);

    /* Loop forever sending and receiving frames periodically. */
    while (1)
    {
        /* Activate reception immediately. See NOTE 4 below. */
        dwt_rxenable(DWT_START_RX_IMMEDIATE);

        /* Poll until a frame is properly received or an error occurs. See NOTE 5 below.
         * STATUS register is 5 bytes long but, as the events we are looking at are in the lower bytes of the register, we can use this simplest API
         * function to access it. */
        while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_ERR)))
        { led1_toogle(); };

        if (status_reg & SYS_STATUS_RXFCG)
        {
            /* A frame has been received, read it into the local buffer. */
            frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
            if (frame_len <= FRAME_LEN_MAX)
            {
                dwt_readrxdata(rx_buffer, frame_len, 0);
				led2_toogle();
				printf("%s\n",rx_buffer);
            }

            /* TESTING BREAKPOINT LOCATION #1 */

            /* Clear good RX frame event in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

            /* Validate the frame is the one expected as sent by "TX then wait for a response" example. */
            if ((frame_len == 14) && (rx_buffer[0] == 0xC5) && (rx_buffer[10] == 0x43) && (rx_buffer[11] == 0x2))
            {
                int i;

                /* Copy source address of blink in response destination address. */
                for (i = 0; i < 8; i++)
                {
                    tx_msg[DATA_FRAME_DEST_IDX + i] = rx_buffer[BLINK_FRAME_SRC_IDX + i];
                }

                /* Write response frame data to DW1000 and prepare transmission. See NOTE 6 below.*/
                dwt_writetxdata(sizeof(tx_msg), tx_msg, 0); /* Zero offset in TX buffer. */
                dwt_writetxfctrl(sizeof(tx_msg), 0, 0); /* Zero offset in TX buffer, no ranging. */

                /* Send the response. */
                dwt_starttx(DWT_START_TX_IMMEDIATE);

                /* Poll DW1000 until TX frame sent event set. */
                while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
                { led3_toogle(); };

                /* Clear TX frame sent event. */
                dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

                /* Increment the data frame sequence number (modulo 256). */
                tx_msg[DATA_FRAME_SN_IDX]++;
            }
        }
        else
        {
            /* Clear RX error events in the DW1000 status register. */
            dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_ERR);
        }
    }
}

#endif //End of #ifdef DWT_RX_SEND_RESPOND

#ifdef DWT_DS_TWR_INIT
		/* Inter-ranging delay period, in milliseconds. */
	#define RNG_DELAY_MS 1000

	/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
	static dwt_config_t config = {
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

	/* Default antenna delay values for 64 MHz PRF. See NOTE 1 below. */
	#define TX_ANT_DLY 16436
	#define RX_ANT_DLY 16436

	/* Frames used in the ranging process. See NOTE 2 below. */
	static uint8 tx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x21, 0, 0};
	static uint8 rx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0x10, 0x02, 0, 0, 0, 0};
	static uint8 tx_final_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
	#define ALL_MSG_COMMON_LEN 10
	/* Indexes to access some of the fields in the frames defined above. */
	#define ALL_MSG_SN_IDX 2
	#define FINAL_MSG_POLL_TX_TS_IDX 10
	#define FINAL_MSG_RESP_RX_TS_IDX 14
	#define FINAL_MSG_FINAL_TX_TS_IDX 18
	#define FINAL_MSG_TS_LEN 4
	/* Frame sequence number, incremented after each transmission. */
	static uint8 frame_seq_nb = 0;

	/* Buffer to store received response message.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	#define RX_BUF_LEN 20
	static uint8 rx_buffer[RX_BUF_LEN];

	/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
	static uint32 status_reg = 0;

	/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
	 * 1 uus = 512 / 499.2 탎 and 1 탎 = 499.2 * 128 dtu. */
	#define UUS_TO_DWT_TIME 65536

	/* Delay between frames, in UWB microseconds. See NOTE 4 below. */
	/* This is the delay from the end of the frame transmission to the enable of the receiver, as programmed for the DW1000's wait for response feature. */
	#define POLL_TX_TO_RESP_RX_DLY_UUS 150
	/* This is the delay from Frame RX timestamp to TX reply timestamp used for calculating/setting the DW1000's delayed TX function. This includes the
	 * frame length of approximately 2.66 ms with above configuration. */
	#define RESP_RX_TO_FINAL_TX_DLY_UUS 3100
	/* Receive response timeout. See NOTE 5 below. */
	#define RESP_RX_TIMEOUT_UUS 2700
	/* Preamble timeout, in multiple of PAC size. See NOTE 6 below. */
	#define PRE_TIMEOUT 8

	/* Time-stamps of frames transmission/reception, expressed in device time units.
	* As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
	static uint64_t poll_tx_ts;
	static uint64_t resp_rx_ts;
	static uint64_t final_tx_ts;

	/* Declaration of static functions. */
	static uint64_t get_tx_timestamp_u64(void);
	static uint64_t get_rx_timestamp_u64(void);
	static void final_msg_set_ts(uint8 *ts_field, uint64_t ts);
	
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
	
	
	void dwt_ds_twr_init(void)
	{
		

		if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
		{
			printf("INIT FAILED");
			while (1)
			{ };
		}
		fast_spi();

		/* Configure DW1000. See NOTE 7 below. */
		dwt_configure(&config);
		dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
		/* Apply default antenna delay value. See NOTE 1 below. */
		dwt_setrxantennadelay(RX_ANT_DLY);
		dwt_settxantennadelay(TX_ANT_DLY);

		/* Set expected response's delay and timeout. See NOTE 4, 5 and 6 below.
		 * As this example only handles one incoming frame with always the same delay and timeout, those values can be set here once for all. */
		dwt_setrxaftertxdelay(150); //POLL_TX_TO_RESP_RX_DLY_UUS
		dwt_setrxtimeout(20000); //RESP_RX_TIMEOUT_UUS
		dwt_setpreambledetecttimeout(50); //PRE_TIMEOUT

		/* Loop forever initiating ranging exchanges. */
		while (1)
		{
			/* Write frame data to DW1000 and prepare transmission. See NOTE 8 below. */
			tx_poll_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
			dwt_writetxdata(sizeof(tx_poll_msg), tx_poll_msg, 0); /* Zero offset in TX buffer. */
			dwt_writetxfctrl(sizeof(tx_poll_msg), 0, 1); /* Zero offset in TX buffer, ranging. */

			/* Start transmission, indicating that a response is expected so that reception is enabled automatically after the frame is sent and the delay
			 * set by dwt_setrxaftertxdelay() has elapsed. */
			dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED);

			/* We assume that the transmission is achieved correctly, poll for reception of a frame or error/timeout. See NOTE 9 below. */
			while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
			{ led1_toogle();};

			/* Increment frame sequence number after transmission of the poll message (modulo 256). */
			frame_seq_nb++;

			if (status_reg & SYS_STATUS_RXFCG)
			{
				led2_toogle();
				uint32 frame_len;

				/* Clear good RX frame event and TX frame sent in the DW1000 status register. */
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);

				/* A frame has been received, read it into the local buffer. */
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
				if (frame_len <= RX_BUF_LEN)
				{
					dwt_readrxdata(rx_buffer, frame_len, 0);
					led3_toogle();
				}

				/* Check that the frame is the expected response from the companion "DS TWR responder" example.
				 * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
				rx_buffer[ALL_MSG_SN_IDX] = 0;
				if (memcmp(rx_buffer, rx_resp_msg, ALL_MSG_COMMON_LEN) == 0)
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
					final_tx_ts = (((uint64_t)(final_tx_time & 0xFFFFFFFEUL)) << 8) + TX_ANT_DLY;

					/* Write all timestamps in the final message. See NOTE 11 below. */
					final_msg_set_ts(&tx_final_msg[FINAL_MSG_POLL_TX_TS_IDX], poll_tx_ts);
					final_msg_set_ts(&tx_final_msg[FINAL_MSG_RESP_RX_TS_IDX], resp_rx_ts);
					final_msg_set_ts(&tx_final_msg[FINAL_MSG_FINAL_TX_TS_IDX], final_tx_ts);

					/* Write and send final message. See NOTE 8 below. */
					tx_final_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
					dwt_writetxdata(sizeof(tx_final_msg), tx_final_msg, 0); /* Zero offset in TX buffer. */
					dwt_writetxfctrl(sizeof(tx_final_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
					ret = dwt_starttx(DWT_START_TX_DELAYED);

					/* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 12 below. */
					if (ret == DWT_SUCCESS)
					{
						/* Poll DW1000 until TX frame sent event set. See NOTE 9 below. */
						while (!(dwt_read32bitreg(SYS_STATUS_ID) & SYS_STATUS_TXFRS))
						{led4_toogle(); };

						/* Clear TXFRS event. */
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_TXFRS);

						/* Increment frame sequence number after transmission of the final message (modulo 256). */
						frame_seq_nb++;
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

			/* Execute a delay between ranging exchanges. */
			delay_ms(RNG_DELAY_MS);
		}
	}
#endif //end #ifdef DWT_DS_TWR_INIT

#ifdef DWT_DS_TWR_RESP
	/* Default communication configuration. We use here EVK1000's default mode (mode 3). */
	static dwt_config_t config = {
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

	/* Default antenna delay values for 64 MHz PRF. See NOTE 1 below. */
	#define TX_ANT_DLY 16436
	#define RX_ANT_DLY 16436

	/* Frames used in the ranging process. See NOTE 2 below. */
	static uint8 rx_poll_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x21, 0, 0};
	static uint8 tx_resp_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'V', 'E', 'W', 'A', 0x10, 0x02, 0, 0, 0, 0};
	static uint8 rx_final_msg[] = {0x41, 0x88, 0, 0xCA, 0xDE, 'W', 'A', 'V', 'E', 0x23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
	#define ALL_MSG_COMMON_LEN 10
	/* Index to access some of the fields in the frames involved in the process. */
	#define ALL_MSG_SN_IDX 2
	#define FINAL_MSG_POLL_TX_TS_IDX 10
	#define FINAL_MSG_RESP_RX_TS_IDX 14
	#define FINAL_MSG_FINAL_TX_TS_IDX 18
	#define FINAL_MSG_TS_LEN 4
	/* Frame sequence number, incremented after each transmission. */
	static uint8 frame_seq_nb = 0;

	/* Buffer to store received messages.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	#define RX_BUF_LEN 24
	static uint8 rx_buffer[RX_BUF_LEN];

	/* Hold copy of status register state here for reference so that it can be examined at a debug breakpoint. */
	static uint32 status_reg = 0;

	/* UWB microsecond (uus) to device time unit (dtu, around 15.65 ps) conversion factor.
	 * 1 uus = 512 / 499.2 탎 and 1 탎 = 499.2 * 128 dtu. */
	#define UUS_TO_DWT_TIME 65536

	/* Delay between frames, in UWB microseconds. See NOTE 4 below. */
	/* This is the delay from Frame RX timestamp to TX reply timestamp used for calculating/setting the DW1000's delayed TX function. This includes the
	 * frame length of approximately 2.46 ms with above configuration. */
	#define POLL_RX_TO_RESP_TX_DLY_UUS 3900
	/* This is the delay from the end of the frame transmission to the enable of the receiver, as programmed for the DW1000's wait for response feature. */
	#define RESP_TX_TO_FINAL_RX_DLY_UUS 500
	/* Receive final timeout. See NOTE 5 below. */
	#define FINAL_RX_TIMEOUT_UUS 3300
	/* Preamble timeout, in multiple of PAC size. See NOTE 6 below. */
	#define PRE_TIMEOUT 8

	/* Timestamps of frames transmission/reception.
	 * As they are 40-bit wide, we need to define a 64-bit int type to handle them. */
	static uint64_t poll_rx_ts;
	static uint64_t resp_tx_ts;
	static uint64_t final_rx_ts;

	/* Speed of light in air, in metres per second. */
	#define SPEED_OF_LIGHT 299702547

	/* Hold copies of computed time of flight and distance here for reference so that it can be examined at a debug breakpoint. */
	static double tof;
	static double distance;

	/* String used to display measured distance on LCD screen (16 characters maximum). */
	char dist_str[16] = {0};

	/* Declaration of static functions. */
	static uint64_t get_tx_timestamp_u64(void);
	static uint64_t get_rx_timestamp_u64(void);
	static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts);
	
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
			ts |= (uint64_t)(ts_tab[i]);
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
			ts |= (uint64_t)(ts_tab[i]);
		}
		return ts;
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
	
	void dwt_ds_twr_resp(void)
	{
		if (dwt_initialise(DWT_LOADUCODE) == DWT_ERROR)
		{
			printf("INIT FAILED");
			while (1)
			{ };
		}
		fast_spi();

		/* Configure DW1000. See NOTE 7 below. */
		dwt_configure(&config);
		dwt_setleds(DWT_LEDS_ENABLE|DWT_LEDS_INIT_BLINK);
	
		/* Apply default antenna delay value. See NOTE 1 below. */
		dwt_setrxantennadelay(RX_ANT_DLY);
		dwt_settxantennadelay(TX_ANT_DLY);

		/* Set preamble timeout for expected frames. See NOTE 6 below. */
		dwt_setpreambledetecttimeout(PRE_TIMEOUT);

		/* Loop forever responding to ranging requests. */
		while (1)
		{
			/* Clear reception timeout to start next ranging process. */
			dwt_setrxtimeout(0);

			/* Activate reception immediately. */
			dwt_rxenable(DWT_START_RX_IMMEDIATE);

			/* Poll for reception of a frame or error/timeout. See NOTE 8 below. */
			while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
			{led4_toogle(); };

			if (status_reg & SYS_STATUS_RXFCG)
			{
				uint32 frame_len;

				/* Clear good RX frame event in the DW1000 status register. */
				dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG);

				/* A frame has been received, read it into the local buffer. */
				frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFL_MASK_1023;
				if (frame_len <= RX_BUFFER_LEN)
				{
					dwt_readrxdata(rx_buffer, frame_len, 0);
					printf("%s\n",rx_buffer);
				}

				/* Check that the frame is a poll sent by "DS TWR initiator" example.
				 * As the sequence number field of the frame is not relevant, it is cleared to simplify the validation of the frame. */
				rx_buffer[ALL_MSG_SN_IDX] = 0;
				
				printf("Step 0\n");
				
				if (memcmp(rx_buffer, rx_poll_msg, ALL_MSG_COMMON_LEN) == 0)
				{
					printf("Step 1\n");
					uint32 resp_tx_time;
					int ret;

					/* Retrieve poll reception timestamp. */
					poll_rx_ts = get_rx_timestamp_u64();

					/* Set send time for response. See NOTE 9 below. */
					resp_tx_time = (uint64_t)(poll_rx_ts + (POLL_RX_TO_RESP_TX_DLY_UUS * UUS_TO_DWT_TIME)) >> 8;
					dwt_setdelayedtrxtime(resp_tx_time); 
					
					printf("%lX%lX vs %lX%lX \n", (uint32_t)(poll_rx_ts >> 32), (uint32_t)(poll_rx_ts),(uint32_t)(resp_tx_time >> 32),(uint32_t)(resp_tx_time));
					
					/* Set expected delay and timeout for final message reception. See NOTE 4 and 5 below. */
					dwt_setrxaftertxdelay(RESP_TX_TO_FINAL_RX_DLY_UUS);  
					dwt_setrxtimeout(FINAL_RX_TIMEOUT_UUS); 

					/* Write and send the response message. See NOTE 10 below.*/
					tx_resp_msg[ALL_MSG_SN_IDX] = frame_seq_nb;
					dwt_writetxdata(sizeof(tx_resp_msg), tx_resp_msg, 0); /* Zero offset in TX buffer. */
					dwt_writetxfctrl(sizeof(tx_resp_msg), 0, 1); /* Zero offset in TX buffer, ranging. */
					
					ret = dwt_starttx(DWT_START_TX_DELAYED | DWT_RESPONSE_EXPECTED);
				
					/* If dwt_starttx() returns an error, abandon this ranging exchange and proceed to the next one. See NOTE 11 below. */
					if (ret == DWT_ERROR)
					{
						printf("DWT_ERROR\n");
						continue;
					}

					/* Poll for reception of expected "final" frame or error/timeout. See NOTE 8 below. */
					while (!((status_reg = dwt_read32bitreg(SYS_STATUS_ID)) & (SYS_STATUS_RXFCG | SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR)))
					{ 
						led1_toogle();
					}
					printf("Step 2\n");
					/* Increment frame sequence number after transmission of the response message (modulo 256). */
					frame_seq_nb++;

					if (status_reg & SYS_STATUS_RXFCG)
					{
						printf("Step 3\n");
						led2_toogle();
						/* Clear good RX frame event and TX frame sent in the DW1000 status register. */
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_RXFCG | SYS_STATUS_TXFRS);

						/* A frame has been received, read it into the local buffer. */
						frame_len = dwt_read32bitreg(RX_FINFO_ID) & RX_FINFO_RXFLEN_MASK;
						if (frame_len <= RX_BUF_LEN)
						{
							dwt_readrxdata(rx_buffer, frame_len, 0);
							printf("%s\n",rx_buffer);
						}

						/* Check that the frame is a final message sent by "DS TWR initiator" example.
						 * As the sequence number field of the frame is not used in this example, it can be zeroed to ease the validation of the frame. */
						rx_buffer[ALL_MSG_SN_IDX] = 0;
						if (memcmp(rx_buffer, rx_final_msg, ALL_MSG_COMMON_LEN) == 0)
						{
							led3_toogle();
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

							/* Display computed distance on LCD. */
							printf("DIST: %d mm\n", (uint16_t)(distance*100));
						}
					}
					else
					{
						printf("Step 3 failed\n");
						/* Clear RX error/timeout events in the DW1000 status register. */
						dwt_write32bitreg(SYS_STATUS_ID, SYS_STATUS_ALL_RX_TO | SYS_STATUS_ALL_RX_ERR);

						/* Reset RX to properly reinitialise LDE operation. */
						dwt_rxreset();
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
		}
		}
	
#endif //end #ifdef DWT_DS_TWR_RESP



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
	
	//dwt_rx_send_respond();
	//dwt_tx_wait_respond();
	
	
	dwt_show_sys_info();
	
	
	#ifdef DWT_SIMPLE_TX
		dwt_send_example();
	#endif
	#ifdef DWT_SIMPLE_RX
		dwt_receive_example();
	#endif
	#ifdef DWT_DS_TWR_INIT
		dwt_ds_twr_init();
	#endif
	#ifdef DWT_DS_TWR_RESP 
		dwt_ds_twr_resp();
	#endif
	
	#ifdef DWT_TX_WAIT_RESP_INTERRUPTS
		dwt_tx_wait_resp_interrupts();
	#endif
	
	#ifdef DWT_RX_SEND_RESPOND
		dwt_rx_send_resp();
    #endif
	//dwt_send_example();
	//dwt_receive_example();
	
	
	
	
	
	uint32_t dev_id;
	
	for (;;) //spi test
	{
		dev_id = dwt_readdevid();
		printf("%#04X %#04X\n", (uint16_t)(dev_id>>16),(uint16_t)dev_id);
		led1_toogle();
		delay_ms(1000);
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
