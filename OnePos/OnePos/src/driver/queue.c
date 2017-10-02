/*
 * queue.c
 *
 * Created: 12/5/2016 9:41:39 PM
 *  Author: Nick
 */ 


/* ============= Modulo de colas (buffer circular) ================ */

#include "../include/queue.h"
#include "../onepos.h"

void queue_init (volatile Queue *cp, volatile char *bp, Queue_Num max)
{
	cp->bp = bp;
	cp->max = max;
	cp->num = cp->in = cp->out = 0;
}

uint8_t queue_add (volatile Queue *cp,char data)
{
	if (cp->num < cp->max)
	{
		cp->bp[cp->in] = data;
		++(cp->num);
		++(cp->in);
		if (cp->in >= cp->max)
		cp->in = 0;
		return QUEUE_OK;
	}
	else
	{
		led4_toogle(); //error check
		return QUEUE_ERROR;
	}
	
}

uint8_t queue_add_str(volatile Queue *cp,char * data)
{
	while (*data)
	{
		if (queue_add(cp, *data) == QUEUE_ERROR)
		{
			return QUEUE_ERROR;
		}
		data++;
	}
	return QUEUE_OK;
}

uint8_t queue_remove(volatile Queue *cp,char *dp)
{
	if (cp->num)
	{
		*dp = cp->bp[cp->out];
		--(cp->num);
		++(cp->out);
		if (cp->out >= cp->max)
		cp->out = 0;
		return QUEUE_OK;
	}
	else
	return QUEUE_ERROR;
}

Queue_Num queue_current_size (volatile const Queue *cp)
{
	return cp->num;
}

Queue_Num queue_available_spaces(volatile const Queue *cp)
{
	return (cp->max - cp->num);
}

uint8_t queue_is_empty(volatile const Queue *cp)
{
	if (cp->num == 0)
	{
		return 1;
	}
	return 0;
}
uint8_t queue_is_full(volatile const Queue *cp)
{
	if (cp->max == cp->num)
	{
		return 1;
	}
	return 0;
}
