/*
 * onepos.c
 *
 * Created: 9/22/2017 8:37:02 AM
 *  Author: Nick
 */ 

#include "../onepos.h"

void init_onepos(void)
{
	sysclk_init();
	ioport_init();
	delay_ms(200);
	
	/************************************************************************/
	/* Direcion of IOs                                                                     */
	/************************************************************************/
	ioport_set_pin_dir(SW1,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(SW2,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(LED1,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED2,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED3,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED4,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(RS485_TX_EN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(RS485_TX,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(RS485_RX,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(BLE_TX,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(BLE_RX,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(UWB_CS,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(UWB_MOSI,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(UWB_CLK,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(UWB_MISO,IOPORT_DIR_INPUT);
	ioport_set_pin_mode(UWB_MISO,IOPORT_MODE_PULLDOWN);
	ioport_set_pin_dir(UWB_INTERRUPT,IOPORT_DIR_INPUT);
	
	
	//PORTE.PIN0CTRL = PORT_ISC_LEVEL_gc;
	//PORTE.INT0MASK = PIN0_bm;
	//PORTE.INTCTRL = PORT_INT0LVL_LO_gc;
	
	#ifdef ENABLE_RTC
	// 	rtc_init();
	#endif
	#ifdef ENABLE_USB
	usb_init();
	#endif
	
	
	//pmic_init();
	//pmic_set_scheduling(PMIC_SCH_ROUND_ROBIN);
	//cpu_irq_enable();
	
}

void init_animation(void)
{
	led1(ON);
	delay_ms(40);
	led2(ON);
	delay_ms(40);
	led3(ON);
	delay_ms(40);
	led4(ON);
	delay_ms(40);
	
	led1(OFF);
	delay_ms(40);
	led2(OFF);
	delay_ms(40);
	led3(OFF);
	delay_ms(40);
	led4(OFF);
	delay_ms(40);
	
	led1(ON);
	delay_ms(80);
	led2(ON);
	delay_ms(80);
	led3(ON);
	delay_ms(80);
	led4(ON);
	delay_ms(80);
	
	led1(OFF);
	delay_ms(80);
	led2(OFF);
	delay_ms(80);
	led3(OFF);
	delay_ms(80);
	led4(OFF);
	delay_ms(80);
	
	led1(ON);
	delay_ms(120);
	led2(ON);
	delay_ms(120);
	led3(ON);
	delay_ms(120);
	led4(ON);
	delay_ms(120);
	
	led1(OFF);
	delay_ms(120);
	led2(OFF);
	delay_ms(120);
	led3(OFF);
	delay_ms(120);
	led4(OFF);
	delay_ms(120);
	
	led1(ON);
	delay_ms(160);
	led2(ON);
	delay_ms(160);
	led3(ON);
	delay_ms(160);
	led4(ON);
	delay_ms(160);
	
	led1(OFF);
	delay_ms(160);
	led2(OFF);
	delay_ms(160);
	led3(OFF);
	delay_ms(160);
	led4(OFF);
	delay_ms(160);
}