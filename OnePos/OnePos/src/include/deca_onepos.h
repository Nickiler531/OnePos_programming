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
#define DECA_STRING_MSG 0x30 //no standard
#define DECA_ONEPOS_REPORT_MSG 0x31 //No standard



/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
#define ALL_MSG_COMMON_LEN 10
/* Indexes to access some of the fields in the frames defined above. */
#define ALL_MSG_SN_IDX 2
#define FINAL_MSG_POLL_TX_TS_IDX 10
#define FINAL_MSG_RESP_RX_TS_IDX 14
#define FINAL_MSG_FINAL_TX_TS_IDX 18
#define FINAL_MSG_TS_LEN 4


void dwt_onepos_init(uint8_t led_indicators);

uint8_t dwt_string_counter(char * str);

void dwt_show_sys_info(void);



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