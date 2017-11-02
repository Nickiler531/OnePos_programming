/*
 * onepos.h
 *
 * Created: 9/22/2017 8:15:37 AM
 *  Author: Nick
 */ 


#ifndef ONEPOS_H_
#define ONEPOS_H_

// Comment to hide the debug messages
//#define DBG

#define ON 1
#define OFF 0


#include <asf.h>

#include "include/ble.h"
#include "stdio.h"
#include "string.h"

#include "include/usb_serial.h"
#include "include/deca_device_api.h"
#include "include/deca_regs.h"
#include "include/deca_spi.h"
#include "include/deca_param_types.h"
#include "include/onepos_config.h"
#include "include/deca_onepos.h"
//#include "include/deca_examples.h"

//Header 1
#define PR0 IOPORT_CREATE_PIN(PORTR, 0)
#define PR1 IOPORT_CREATE_PIN(PORTR, 1)
#define PA2 IOPORT_CREATE_PIN(PORTA, 2)
#define PA3 IOPORT_CREATE_PIN(PORTA, 3)
#define PA4 IOPORT_CREATE_PIN(PORTA, 4)
#define PA5 IOPORT_CREATE_PIN(PORTA, 5)
#define PB2 IOPORT_CREATE_PIN(PORTB, 2)
#define PB3 IOPORT_CREATE_PIN(PORTB, 3)

//Header 2
#define PE0 IOPORT_CREATE_PIN(PORTE, 0) //UWB interrupt
#define PD5 IOPORT_CREATE_PIN(PORTD, 5)
#define PD4 IOPORT_CREATE_PIN(PORTE, 4)
#define PD1 IOPORT_CREATE_PIN(PORTD, 1)
#define PD0 IOPORT_CREATE_PIN(PORTD, 0)

//User Interface (Leds and buttons)
#define LED1 IOPORT_CREATE_PIN(PORTA, 6)
#define LED2 IOPORT_CREATE_PIN(PORTA, 7)
#define LED3 IOPORT_CREATE_PIN(PORTB, 0)
#define LED4 IOPORT_CREATE_PIN(PORTB, 1)

#define SW1 IOPORT_CREATE_PIN(PORTC, 3)
#define SW2 IOPORT_CREATE_PIN(PORTC, 2)

//RS485 interface
#define RS485_TX_EN IOPORT_CREATE_PIN(PORTE, 1)
#define RS485_TX IOPORT_CREATE_PIN(PORTE, 2)
#define RS485_RX IOPORT_CREATE_PIN(PORTE, 3)

//Bluetooh Low Energy interface
#define BLE_TX IOPORT_CREATE_PIN(PORTD, 2)
#define BLE_RX IOPORT_CREATE_PIN(PORTD, 3)

//UWB interface
#define UWB_CS IOPORT_CREATE_PIN(PORTC, 4)
#define UWB_MOSI IOPORT_CREATE_PIN(PORTC, 5)
#define UWB_MISO IOPORT_CREATE_PIN(PORTC, 6)
#define UWB_CLK IOPORT_CREATE_PIN(PORTC, 7)
#define UWB_INTERRUPT PE0
/************************************************************************/
/* Functions MACROs                                                                     */
/************************************************************************/

#define led1(x) ioport_set_pin_level(LED1,x)
#define led2(x) ioport_set_pin_level(LED2,x)
#define led3(x) ioport_set_pin_level(LED3,x)
#define led4(x) ioport_set_pin_level(LED4,x)

#define led1_toogle() ioport_toggle_pin_level(LED1)
#define led2_toogle() ioport_toggle_pin_level(LED2)
#define led3_toogle() ioport_toggle_pin_level(LED3)
#define led4_toogle() ioport_toggle_pin_level(LED4)

#define sw1_status() !ioport_get_pin_level(SW1) //1 for pressed, 0 for released
#define sw2_status() !ioport_get_pin_level(SW2) //1 for pressed, 0 for released

/************************************************************************/
/* Configuration in initiation. To disable one of the following options, comment it.*/
/************************************************************************/
#define ENABLE_USB
//#define ENABLE_RTC

/************************************************************************/

/************************************************************************/
/* Printf and scan f function can be use with usb or Bluetooth. Choose the communication method you are going to use*/
/************************************************************************/
#define PRINTF_USB

void init_onepos(void);
void init_animation(void);


#endif /* ONEPOS_H_ */