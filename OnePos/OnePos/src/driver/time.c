/*
 * time.c
 *
 * Created: 12/5/2016 9:09:41 AM
 *  Author: Nick
 */ 

#include "../include/time.h"

void time_init (Tm_Control *tcp,
Tm_Period *pp,
Tm_Num n_per,
Tm_Timeout *tp,
Tm_Num n_to,
Tm_check_tick *check_tick)
{
	Tm_Num i;

	/* Condición inicial de la tabla de períodos */
	tcp->pp = pp;
	tcp->n_per = n_per;
	for (i = 0; i < n_per; ++i, ++pp)
	{
		pp->flags = 0;
		pp->counter = pp->period = 0;
	};

	/* Condición inicial de la tabla de timeouts */
	tcp->tp = tp;
	tcp->n_to = n_to;
	for (i = 0; i < n_to; ++i, ++tp)
	*tp = 0;

	/* Se guarda la rutina de verificación y atención del HW */
	tcp->check_tick = check_tick;
}

void time_run(Tm_Control *tcp)
{
	Tm_Num i;
	Tm_Period *pp;
	Tm_Timeout *tp;

	if ( !(tcp->check_tick()) )
		return;

	for (i = 0, pp = tcp->pp; i < tcp->n_per; ++i, ++pp)
	{
		if (pp->flags & TIME_F_PER_ACTIVE)
		{ /* Solo se procesan los períodos activos */
			--(pp->counter);
			if ( !(pp->counter) )
			{ /* Fin de conteo: activa la bandera y reinicia el contador */
				pp->flags |= TIME_F_PER_FC;
				pp->counter = pp->period;
			}
		}
	}

	for (i = 0, tp = tcp->tp; i < tcp->n_to; ++i, ++tp)
		if (*tp)
		/* Solo se procesan los timeouts activos */
		--(*tp);
}

void time_init_period (Tm_Control *tcp,
Tm_Num num,
Tm_Counter periodo)
{
	Tm_Period *pp = tcp->pp + num;  /* &(tcp->pp[num]) */
	
	pp->flags = TIME_F_PER_ACTIVE;
	pp->period = pp->counter = periodo;
}

char time_check_period(const Tm_Control *tcp, Tm_Num num)
{
	if (tcp->pp[num].flags & TIME_F_PER_FC)
	{
		tcp->pp[num].flags&=~TIME_F_PER_FC;
		return 1;
	}
	return 0;
}

void time_stop_period(Tm_Control *tcp, Tm_Num num)
{
	tcp->pp[num].flags&=~TIME_F_PER_ACTIVE;
}

void time_init_timeout(Tm_Control *tcp, Tm_Num num, Tm_Counter timeout)
{
	tcp->tp[num]=timeout;
}

char time_check_timeout(const Tm_Control *tcp, Tm_Num num)
{
	return (tcp->tp[num] == 0x00);
}
