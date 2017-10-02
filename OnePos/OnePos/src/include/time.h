/*
 * time.h
 *
 * Created: 12/5/2016 9:11:37 AM
 *  Author: Nick
 */ 

#ifndef TIME_H_
#define TIME_H_

#include <stdint.h>

typedef uint8_t Tm_Num;
typedef uint32_t Tm_Counter;

typedef struct Tm_Period Tm_Period;
#define TIME_F_PER_FC        0x01
#define TIME_F_PER_ACTIVE    0x80
struct Tm_Period
{
	unsigned char flags;
	Tm_Counter period,
	counter;
};

typedef Tm_Counter Tm_Timeout;

typedef char Tm_check_tick(void);

/* Estructura de control del módulo */
typedef struct Tm_Control Tm_Control;
struct Tm_Control
{
	/* Tabla de períodoc */
	Tm_Period *pp;
	Tm_Num n_per;
	
	/* Tabla de timeouts */
	Tm_Timeout *tp;
	Tm_Num n_to;

	/* Rutina para verificar y atender HW */
	Tm_check_tick *check_tick;
};


/* Definición de condición inicial del módulo */
void time_init(Tm_Control *tcp, Tm_Period *pp, Tm_Num n_per, Tm_Timeout *tp, Tm_Num n_to, Tm_check_tick *atender);

/* Proceso */
void time_run(Tm_Control *tcp);

/* Rutinas de interfaz para período */
void time_init_period(Tm_Control *tcp, Tm_Num num, Tm_Counter periodo);
char time_check_period(const Tm_Control *tcp, Tm_Num num);
void time_stop_period(Tm_Control *tcp, Tm_Num num);

/* Rutinas de interfaz para timeout */
void time_init_timeout(Tm_Control *tcp, Tm_Num num, Tm_Counter timeout);
char time_check_timeout(const Tm_Control *tcp, Tm_Num num);


#endif