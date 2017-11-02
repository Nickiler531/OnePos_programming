/*
 * deca_onepos.h
 *
 * Created: 10/12/2017 11:12:50 AM
 *  Author: Nick
 */ 


#ifndef DECA_ONEPOS_H_
#define DECA_ONEPOS_H_

#include "../onepos.h"

//#define DWT_SIMPLE_RX
//#define DWT_SIMPLE_TX
#define DWT_DS_TWR_INIT
//#define DWT_DS_TWR_RESP
//#define DWT_TX_WAIT_RESP_INTERRUPTS
//#define DWT_RX_SEND_RESPOND


uint8_t dwt_string_counter(char * str);

void dwt_show_sys_info(void);

void dwt_run_examples(void);

#ifdef DWT_SIMPLE_TX

	/* Index to access to sequence number of the blink frame in the tx_msg array. */
	#define BLINK_FRAME_SN_IDX 1

	/* Inter-frame delay period, in milliseconds. */
	#define TX_DELAY_MS 1000

	void dwt_send_example(void);

#endif // end of #ifdef DWT_SIMPLE_SEND

#ifdef DWT_SIMPLE_RX

#define FRAME_LEN_MAX 127

void dwt_receive_example(void);

#endif

#ifdef DWT_TX_WAIT_RESP_INTERRUPTS

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

/* Buffer to store received frame. See NOTE 4 below. */
#define FRAME_LEN_MAX 127



void dwt_tx_wait_resp_interrupts(void);

#endif //endif DWT_TX_WAIT_RESP_INTERRUPTS


#ifdef DWT_RX_SEND_RESPOND


/* Indexes to access to sequence number and destination address of the data frame in the tx_msg array. */
#define DATA_FRAME_SN_IDX 2
#define DATA_FRAME_DEST_IDX 5

/* Inter-frame delay period, in milliseconds. */
#define TX_DELAY_MS 1000

/* Buffer to store received frame. See NOTE 1 below. */
#define FRAME_LEN_MAX 127
/* Index to access to source address of the blink frame in the rx_buffer array. */
#define BLINK_FRAME_SRC_IDX 2

void dwt_rx_send_resp(void);

#endif //End of #ifdef DWT_RX_SEND_RESPOND

#ifdef DWT_DS_TWR_INIT2
		/* Inter-ranging delay period, in milliseconds. */
	#define RNG_DELAY_MS 1000

	/* Default antenna delay values for 64 MHz PRF. See NOTE 1 below. */
	#define TX_ANT_DLY 16436
	#define RX_ANT_DLY 16436

	/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
	#define ALL_MSG_COMMON_LEN 10
	/* Indexes to access some of the fields in the frames defined above. */
	#define ALL_MSG_SN_IDX 2
	#define FINAL_MSG_POLL_TX_TS_IDX 10
	#define FINAL_MSG_RESP_RX_TS_IDX 14
	#define FINAL_MSG_FINAL_TX_TS_IDX 18
	#define FINAL_MSG_TS_LEN 4
	
	/* Buffer to store received response message.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	#define RX_BUF_LEN 20

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
	
	
	void dwt_ds_twr_init(void);
	
#endif //end #ifdef DWT_DS_TWR_INIT

#ifdef DWT_DS_TWR_RESP2
	
	/* Default antenna delay values for 64 MHz PRF. See NOTE 1 below. */
	#define TX_ANT_DLY 16436
	#define RX_ANT_DLY 16436

	
	/* Length of the common part of the message (up to and including the function code, see NOTE 2 below). */
	#define ALL_MSG_COMMON_LEN 10
	/* Index to access some of the fields in the frames involved in the process. */
	#define ALL_MSG_SN_IDX 2
	#define FINAL_MSG_POLL_TX_TS_IDX 10
	#define FINAL_MSG_RESP_RX_TS_IDX 14
	#define FINAL_MSG_FINAL_TX_TS_IDX 18
	#define FINAL_MSG_TS_LEN 4
	

	/* Buffer to store received messages.
	 * Its size is adjusted to longest frame that this example code is supposed to handle. */
	#define RX_BUF_LEN 20

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

	/* Speed of light in air, in metres per second. */
	#define SPEED_OF_LIGHT 299702547

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
	static void final_msg_get_ts(const uint8 *ts_field, uint32 *ts);
	
	void dwt_ds_twr_resp(void);
	
#endif //end #ifdef DWT_DS_TWR_RESP


#endif /* DECA_ONEPOS_H_ */