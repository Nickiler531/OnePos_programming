/*
 * time_config.h
 *
 * Created: 12/6/2016 7:58:16 AM
 *  Author: Nick
 */ 


#ifndef TIME_CONFIG_H_
#define TIME_CONFIG_H_

/************************************************************************/
/* TIME Definitions                                                                     */
/************************************************************************/
enum periods {
	BLINK1_PERIOD,
	GPS_PERIOD,
	SIM808_PERIOD,
	TIME_N_PER
};
enum timeouts {
	TIMEOUT_COMM_START,
	TIMEOUT_COMM_CHAR,
	TIMEOUT_100MS,
	TIMEOUT_300MS,
	TIMEOUT_500MS,
	TIMEOUT_750MS,
	TIMEOUT_1S,
	TIMEOUT_5S,
	TIMEOUT_10S,
	TIME_N_TIMEOUT
};

#define TICK 1062 //Calculated 2,082ms
#define TICK_MS 2.082
#define GPS_PERIOD_t 2000/TICK_MS
#define SIM808_PERIOD_t 3

#define MUX_PERIOD_t 2 //10,41ms

#define BLINK1_t 500/TICK_MS

#define TIMEOUT_100MS_t 100/TICK_MS
#define TIMEOUT_300MS_t 300/TICK_MS
#define TIMEOUT_500MS_t 500/TICK_MS
#define TIMEOUT_750MS_t 750/TICK_MS
#define TIMEOUT_1S_t	1000/TICK_MS
#define TIMEOUT_5S_t	5000/TICK_MS
#define TIMEOUT_10S_t   10000/TICK_MS

Tm_Control time_control;



#endif /* TIME_CONFIG_H_ */