#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <inttypes.h>

#include "init.h"
#include "functions.h"

/*--------------------- LCD Helper Function --------------------------------*/

void lcd_print_int(int8_t line_no, int8_t pos_no, int8_t number)
{
	char s[3];
	sprintf(s,"%3d",number);
	lcd_cursor(line_no,pos_no);
	lcd_string(s);
}
void print_all_ticks(void)
{
	lcd_print_int(1,1,ticks[0]);
	lcd_print_int(1,4,ticks[1]);
	lcd_print_int(1,8,ticks[2]);
	lcd_print_int(1,12,ticks[3]);
	lcd_print_int(2,1,ticks[4]);
	lcd_print_int(2,4,ticks[5]);
}

void initialize_motors()
{
	TCNT1 = 0;
	int8_t return_flag, i;
	m1_fwd;
	m2_fwd;
	m3_fwd;
	m4_fwd;
	m5_fwd;
	m6_fwd;
	for(i=0;i<NUMBER_OF_MOTORS;i++)
	{
		ticks[i] = rotate_dir[i] = tick_reached[i] = motor_initialized[i] = 0;
		motor_offset_flag[i] = 1;
		stop_tick[i] = -50;
	}
	do
	{
		return_flag = (motor_initialized[5]&& motor_initialized[4]&& motor_initialized[3]&& motor_initialized[2]&& motor_initialized[1]&& motor_initialized[0]);
		for (i=0;i<6;i++)
		{
			if (motor_initialized[i])
			{
				switch (i)
				{
					case 0:	m1_stall;break;
					case 1:	m2_stall;break;
					case 2:	m3_stall;break;
					case 3:	m4_stall;break;
					case 4:	m5_stall;break;
					case 5:	m6_stall;break;
				}
			}
		}
	}while(return_flag == 0);
	usart_data = 0;
}

void reach_to_ticks(int8_t final_ticks, int8_t motor_number, uint8_t rotation_dir)
{
	stop_tick[motor_number] = final_ticks;
	rotate_dir[motor_number] = rotation_dir;
	tick_reached[motor_number] = stall_flag;
	//This function takes input the number of ticks and dynamically rotates the motor in the given direction until it reaches the final ticks
	if(rotate_dir[motor_number])
		{
			switch (motor_number)
			{
				case 0:	m1_rev;break;
				case 1:	m2_rev;break;
				case 2:	m3_rev;break;
				case 3:	m4_rev;break;
				case 4:	m5_rev;break;
				case 5:	m6_rev;break;
			}
		}
		else
		{
			switch (motor_number)
			{
				case 0:	m1_fwd;break;
				case 1:	m2_fwd;break;
				case 2:	m3_fwd;break;
				case 3:	m4_fwd;break;
				case 4:	m5_fwd;break;
				case 5:	m6_fwd;break;
			}
		}
}

void motor_test()
{
	m1_fwd;
	_delay_ms(1000);
	m1_stall;
	m2_fwd;
	_delay_ms(1000);
	m2_stall;
	m3_fwd;
	_delay_ms(1000);
	m3_stall;
	m4_fwd;
	_delay_ms(1000);
	m4_stall;
	m5_fwd;
	_delay_ms(1000);
	m5_stall;
	m6_fwd;
	_delay_ms(1000);
	m6_stall;	
}
void encoder_test()
{
	reach_to_ticks(20,m1,fwd);
	while((tick_reached[m1]==0));
	reach_to_ticks(20,m2,fwd);
	while((tick_reached[m2]==0));
	reach_to_ticks(20,m3,fwd);
	while((tick_reached[m3]==0));
	reach_to_ticks(20,m4,fwd);
	while((tick_reached[m4]==0));
	reach_to_ticks(20,m5,fwd);
	while((tick_reached[m5]==0));
	reach_to_ticks(20,m6,fwd);
	while((tick_reached[m6]==0));
}

void ground_left()
{
	TCNT1 = 0;
	reach_to_ticks(24,m1,fwd);
	reach_to_ticks(16,m3,rev);
	reach_to_ticks(24,m5,fwd);
	reach_to_ticks(20,m2,rev);
	reach_to_ticks(20,m4,fwd);
	reach_to_ticks(20,m6,rev);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m1,fwd);
	reach_to_ticks(20,m3,rev);
	reach_to_ticks(20,m5,fwd);
	reach_to_ticks(16,m2,rev);
	reach_to_ticks(24,m4,fwd);
	reach_to_ticks(16,m6,rev);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	//USART1_Transmit_8Bit('C');
	//usart_data = 0;
}
void ground_right()
{
	TCNT1 = 0;
	reach_to_ticks(20,m1,rev);
	reach_to_ticks(20,m3,fwd);
	reach_to_ticks(20,m5,rev);
	reach_to_ticks(24,m2,fwd);
	reach_to_ticks(16,m4,rev);
	reach_to_ticks(24,m6,fwd);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	TCNT1 = 0;
	reach_to_ticks(16,m1,rev);
	reach_to_ticks(24,m3,fwd);
	reach_to_ticks(16,m5,rev);
	reach_to_ticks(20,m2,fwd);
	reach_to_ticks(20,m4,rev);
	reach_to_ticks(20,m6,fwd);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	//USART1_Transmit_8Bit('D');
	//usart_data = 0;
}
void ground_front()
{
	TCNT1 = 0;
	reach_to_ticks(24,m1,fwd);
	reach_to_ticks(24,m3,fwd);
	reach_to_ticks(24,m5,fwd);
	reach_to_ticks(18,m2,fwd);
	reach_to_ticks(18,m4,fwd);
	reach_to_ticks(18,m6,fwd);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	TCNT1 = 0;
	reach_to_ticks(18,m1,fwd);
	reach_to_ticks(18,m3,fwd);
	reach_to_ticks(18,m5,fwd);
	reach_to_ticks(24,m2,fwd);
	reach_to_ticks(24,m4,fwd);
	reach_to_ticks(24,m6,fwd);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	//USART1_Transmit_8Bit('E');
	//usart_data = 0;
}
void ground_reset()
{
	TCNT1 = 0;
	reach_to_ticks(25,m1,rev);
	reach_to_ticks(25,m2,rev);
	reach_to_ticks(25,m3,rev);
	reach_to_ticks(25,m5,rev);
	reach_to_ticks(25,m4,rev);
	reach_to_ticks(25,m6,rev);
	while((tick_reached[m4]==0)|| (tick_reached[m6]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m1]==0)||(tick_reached[m2]==0));
	usart_data = 0;
}
void ground_reverse()
{
	TCNT1 = 0;
	reach_to_ticks(16,m1,rev);
	reach_to_ticks(16,m3,rev);
	reach_to_ticks(16,m5,rev);
	reach_to_ticks(22,m2,rev);
	reach_to_ticks(22,m4,rev);
	reach_to_ticks(22,m6,rev);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	TCNT1 = 0;
	reach_to_ticks(22,m1,rev);
	reach_to_ticks(22,m3,rev);
	reach_to_ticks(22,m5,rev);
	reach_to_ticks(16,m2,rev);
	reach_to_ticks(16,m4,rev);
	reach_to_ticks(16,m6,rev);
	while((tick_reached[m1]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m2]==0)||(tick_reached[m4]==0)||(tick_reached[m6]==0));
	
}
void stairs_left()
{
	TCNT1 = 0;
	reach_to_ticks(24,m1,fwd);
	reach_to_ticks(20,m2,rev);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m1,fwd);
	reach_to_ticks(16,m2,rev);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0));
}
void stairs_right()
{
	TCNT1 = 0;
	reach_to_ticks(20,m1,rev);
	reach_to_ticks(24,m2,fwd);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0));
	TCNT1 = 0;
	reach_to_ticks(16,m1,rev);
	reach_to_ticks(20,m2,fwd);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0));
}
void stairs_front()
{
	TCNT1 = 0;
	reach_to_ticks(22,m5,fwd);
	reach_to_ticks(22,m6,fwd);
	reach_to_ticks(18,m1,fwd);
	reach_to_ticks(18,m2,fwd);
	while((tick_reached[m6]==0)||(tick_reached[m5]==0)||(tick_reached[m1]==0)||(tick_reached[m2]==0));
	TCNT1 = 0;
	reach_to_ticks(18,m5,fwd);
	reach_to_ticks(18,m6,fwd);
	reach_to_ticks(22,m3,fwd);
	reach_to_ticks(22,m4,fwd);
	while((tick_reached[m4]==0)||(tick_reached[m3]==0)||(tick_reached[m6]==0)||(tick_reached[m5]==0));
	TCNT1 = 0;	
	reach_to_ticks(18,m3,fwd);
	reach_to_ticks(18,m4,fwd);
	reach_to_ticks(22,m1,fwd);
	reach_to_ticks(22,m2,fwd);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0)||(tick_reached[m4]==0)||(tick_reached[m3]==0));
}
void stairs_reverse()
{
	TCNT1 = 0;
	reach_to_ticks(20,m5,rev);
	reach_to_ticks(20,m6,rev);
	while((tick_reached[m1]==0)||(tick_reached[m5]==0));
	TCNT1 = 0;
	reach_to_ticks(22,m3,rev);
	reach_to_ticks(22,m4,rev);
	while((tick_reached[m4]==0)||(tick_reached[m3]==0));
	TCNT1 = 0;
	reach_to_ticks(24,m1,rev);
	reach_to_ticks(24,m2,rev);
	while((tick_reached[m2]==0)||(tick_reached[m1]==0));
}
void stairs_reset()
{
	TCNT1 = 0;
	reset_ticks();
	reach_to_ticks(15,m1,fwd);
	reach_to_ticks(15,m2,fwd);
	reach_to_ticks(15,m3,fwd);
	reach_to_ticks(15,m5,fwd);
	reach_to_ticks(15,m4,fwd);
	reach_to_ticks(15,m6,fwd);
	while((tick_reached[m4]==0)|| (tick_reached[m6]==0)||(tick_reached[m3]==0)||(tick_reached[m5]==0)||(tick_reached[m1]==0)||(tick_reached[m2]==0));
	usart_data = 0;
}
void flash_on()
{
	flashOn1;
	flashOn2;
	//USART1_Transmit_8Bit('A');
	usart_data = 0;
}
void flash_off()
{
	flashOff1;
	flashOff2;
	//USART1_Transmit_8Bit('B');
	usart_data = 0;
}
void reset_ticks()
{
	TCNT1 = 0;
	stall_flag = 0;
	for(int8_t i=0;i<NUMBER_OF_MOTORS;i++)
	{
		tick_reached[i] = motor_initialized[i] = 0;
	}
}
void leaping()
{
	TCNT1 = 0;
	reach_to_ticks(10,m5,fwd);
	reach_to_ticks(10,m6,fwd);
	reach_to_ticks(10,m4,fwd);
	reach_to_ticks(10,m3,fwd);
	while((tick_reached[m6]==0)||(tick_reached[m5]==0)||(tick_reached[m3]==0)||(tick_reached[m4]==0));
}
void move_forward_limp()
{
	TCNT1 = 0;
	reach_to_ticks(20,m1,fwd);
	while((tick_reached[m1]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m6,fwd);
	while((tick_reached[m6]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m2,fwd);
	while((tick_reached[m2]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m4,fwd);
	reach_to_ticks(15,m2,fwd);
	reach_to_ticks(15,m3,fwd);
	while((tick_reached[m4]==0)||(tick_reached[m3]==0)||(tick_reached[m1]==0));
	TCNT1 = 0;
	reach_to_ticks(20,m3,fwd);
	while((tick_reached[m3]==0));
}
