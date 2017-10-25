/*
 * deca_onepos.h
 *
 * Created: 10/12/2017 5:45:12 PM
 *  Author: CEA-IoT_NVS
 */ 


#ifndef DECA_ONEPOS_H_
#define DECA_ONEPOS_H_

#include "../onepos.h"

//ISO/IEC 24730-62
#define DECA_POLL_MSG 0x61
#define DECA_BLINK_MSG 0xC5
#define DECA_RANG_INIT_MSG 0x020
#define DECA_RESP_MSG 0x050
#define DECA_FINAL_MSG 0x69

#define DECA_STRING_MSG 0x30 //non standard
#define DECA_ONEPOS_REPORT_MSG 0x31 //Non standard
#define DECA_ACK_MSG 0x32 //non standar

/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
#define ALL_MSG_COMMON_LEN 10
/* Indexes to access some of the fields in the frames defined above. */
#define ALL_MSG_SN_IDX 2
#define FINAL_MSG_POLL_TX_TS_IDX 10
#define FINAL_MSG_RESP_RX_TS_IDX 14
#define FINAL_MSG_FINAL_TX_TS_IDX 18
#define FINAL_MSG_TS_LEN 4

/* Buffer to store received frame. See NOTE 4 below. */
#define FRAME_LEN_MAX 127

/* Current inter-frame delay period.
 * This global static variable is also used as the mechanism to signal events to the background main loop from the interrupt handler callbacks,
 * which set it to positive delay values. */
static int32 tx_delay_ms = -1;


int8 rx_flag;

enum rx_flag_status
{
	UWB_RX_OK,
	UWB_RX_TO,
	UWB_RX_FAILED,
	UWB_RX_OK_BRDCST,
	UWB_RX_OK_NOTFORME,
	UWB_RX_ACK
};

/* Buffer to store received frame. See NOTE 4 below. */
static uint8 rx_buffer[FRAME_LEN_MAX];

typedef struct uwb_rx_msg_str
{
	uint8_t seq_number;
	uint16_t pan_id;
	uint16_t dest_address;
	uint16_t source_address;
	uint8_t msg_len;
	char msg[FRAME_LEN_MAX];
}uwb_rx_msg_str;

uwb_rx_msg_str uwb_rx_message;

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

/* Buffer to store received frame. See NOTE 4 below. */
#define FRAME_LEN_MAX 127



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



void dwt_onepos_init(uint8_t led_indicators);

uint8_t dwt_string_counter(char * str);

void dwt_show_sys_info(void);

uint8_t dwt_send_msg_w_ack(char * msg, uint16_t dest_address);

uint8_t dwt_receive_msg_w_ack(void);

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
static uint64_t get_tx_timestamp_u64(void);

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
static uint64_t get_rx_timestamp_u64(void);

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
static void final_msg_set_ts(uint8 *ts_field, uint64_t ts);






#endif /* DECA_ONEPOS_H_ */