/*
 * queue.h
 *
 * Created: 12/5/2016 9:41:52 PM
 *  Author: Nick
 */ 


#ifndef QUEUE_H_
#define QUEUE_H_

#include <stddef.h>
#include <stdint.h>

/* ------------------- Tipos de datos -------------------- */
// Numero de datos
typedef uint16_t Queue_Num;

/* Estructura de cola */
typedef struct Queue
{
	volatile char *bp;	// Apuntador al buffer de datos
	Queue_Num max,	// Longitud de la cola
	num,	// Numero de bytes en la cola
	in,	// Indice de entrada
	out;	// Indice de salida
}Queue;

#define QUEUE_OK 1
#define QUEUE_ERROR 0
/* ----------------------- Rutinas ------------------------ */

void queue_init (volatile Queue *cp, volatile char *bp, Queue_Num max);

uint8_t queue_add (volatile Queue *cp,char data);

uint8_t queue_add_str(volatile Queue *cp,char * data);

uint8_t queue_remove(volatile Queue *cp,char *dp);

Queue_Num queue_current_size (volatile const Queue *cp);

Queue_Num queue_available_spaces(volatile const Queue *cp);

uint8_t queue_is_empty(volatile const Queue *cp);

uint8_t queue_is_full(volatile const Queue *cp);

#endif /* QUEUE_H_ */