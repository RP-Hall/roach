#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>

#include "USART2560.h"
#include "USART2560.c"
#include "lcd.c"
#include "init.h"
#include "functions.h"

volatile int8_t usart_data = 0,stall_flag = 0;

/*--------------------- Interrupt Service Routines -------------------------*/
ISR(INT0_vect)
{
	if(rotate_dir[0])
		ticks[0]++;
	else
		ticks[0]--;
	
	if(ticks[0] == 30)
		ticks[0] = 0;
	else if(ticks[0] == -1)
		ticks[0] = 29;
	
	if(stop_tick[0] == ticks[0])
	{
		m1_stall;
		tick_reached[0] = 1;
	}
}
ISR(INT1_vect)
{
	if(rotate_dir[1])
		ticks[1]++;
	else
		ticks[1]--;
	
	if(ticks[1] == 30)
		ticks[1] = 0;
	else if(ticks[1] == -1)
		ticks[1] = 29;
	
	if(stop_tick[1] == ticks[1])
	{
		m2_stall;
		tick_reached[1]=1;
	}
}
ISR(INT2_vect)
{
	if(rotate_dir[2])
		ticks[2]++;
	else
		ticks[2]--;
	
	if(ticks[2] == 30)
		ticks[2] = 0;
	else if(ticks[2] == -1)
		ticks[2] = 29;
	
	if(stop_tick[2] == ticks[2])
	{
		m3_stall;
		tick_reached[2]=1;
	}
}
ISR(INT3_vect)
{
	if(rotate_dir[3])
		ticks[3]++;
	else
		ticks[3]--;
	
	if(ticks[3] == 30)
		ticks[3] = 0;
	else if(ticks[3] == -1)
		ticks[3] = 29;
	
	if(stop_tick[3] == ticks[3])
	{
		m4_stall;
		tick_reached[3] = 1;
	}
}
ISR(INT4_vect)
{
	if(rotate_dir[4])
		ticks[4]++;
	else
		ticks[4]--;
	
	if(ticks[4] == 30)							//1 teeth broken
		ticks[4] = 0;
	else if(ticks[4] == -1)
		ticks[4] = 29;
	
	if(stop_tick[4] == ticks[4])
	{
		m5_stall;
		tick_reached[4]=1;
	}
}
ISR(INT5_vect)
{
	if(rotate_dir[5])
		ticks[5]++;
	else
		ticks[5]--;
	
	if(ticks[5] == 30)
		ticks[5] = 0;
	else if(ticks[5] == -1)
		ticks[5] = 29;
	
	if(stop_tick[5] == ticks[5])
	{
		m6_stall;
		tick_reached[5]=1;
	}
}
ISR(BADISR_vect)
{
	return;
}

ISR(PCINT2_vect)
{
	int8_t i;
	for(i=0; i<6; i++)
	{
		if(PINK & (1<<i))
		{
			if(!motor_offset_flag[i])
			{
				motor_initialized[i] = 1;
				motor_offset_flag[i] = 1;
				switch (i)
				{
					case 0 : ticks[i] = m1_offset;break;
					case 1 : ticks[i] = m2_offset;break;
					case 2 : ticks[i] = m3_offset;break;
					case 3 : ticks[i] = m4_offset;break;
					case 4 : ticks[i] = m5_offset;break;
					case 5 : ticks[i] = m6_offset;break;
					default:;
				}
			}				
		}
		else
			motor_offset_flag[i] = 0;
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(usart_data!=0)
	{
		for(int8_t i=0;i<NUMBER_OF_MOTORS;i++)
		{
			tick_reached[i] = 1;
			motor_initialized[i] = 1;
		}
	}			
}


int main(void)
{ 
	_delay_ms(2000);
	init();
	initialize_motors();
	
	/* CONVENSION TO RUN THE MOTORS
	=> Set the rotation direction
	=> call the function reach_to_ticks with "final_ticks" and "motor_number" as arguments*/
	while(1)
 	 {
		switch (usart_data)
		{
			case 'a': reset_ticks();flash_on();break;
			case 'b': reset_ticks();flash_off();break;
			case 'c': reset_ticks();ground_left();break;
			case 'd': reset_ticks();ground_right();break;
			case 'e': reset_ticks();ground_front();break;
			case 'f': reset_ticks();ground_reset();break;
			case 'g': reset_ticks();stairs_left();break;
			case 'h': reset_ticks();stairs_right();break;
			case 'i': reset_ticks();stairs_front();break;
			case 'j': reset_ticks();stairs_reset();break;
			case 'k': reset_ticks();break;
			case 'n': reset_ticks();ground_reverse();break;
			case 'o': reset_ticks();stairs_reverse();break;
			default: break;
			
		}
	 }
}