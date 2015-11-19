#define F_CPU 14745600UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <inttypes.h>


#include "lcd.c"
/*
rotate = 1 : rev
rotate = 0 : fwd
*/
/*
--------------- ENCODER PINS -------------------
INT0 PD0
INT1 PD1
INT2 PD2
INT3 PD3

INT4 PE4
INT5 PE5
INT6 PE6
INT7 PE7

INT 0,1,2,3 are controlled by EICRA - External Interrupt Control Register A
INT 4,5,6,7 are controlled by EICRB - External Interrupt COntrol Register B

The Interrupt can be enabled by the register EIMSK - External Interrupt Mask Register

--------------- MOTOR INPUT PINS ----------------------
PORTA
0 - I2 - L
1 - I1 - L
2 - I4 - L
3 - I3 - L
4 - I3 - R
5 - I4 - R
6 - I1 - R
7 - I2 - R

PORTB
4 - I3 - L
5 - I4 - L
6 - I1 - R
7 - I2 - R

*/

/*--------------------- MACROS ------------------------*/
#define NUMBER_OF_MOTORS	6
#define fwd 0
#define rev 1
#define m1 0
#define m2 1
#define m3 2
#define m4 3
#define m5 4
#define m6 5

#define m1_DDR				DDRA
#define m1_PORT				PORTA
#define m1_1				0
#define m1_2				1
#define m1_stall			m1_PORT |= (1<<m1_1)|(1<<m1_2)
#define m1_rev				m1_PORT |= (1<<m1_1);m1_PORT &= ~(1<<m1_2)
#define m1_fwd				m1_PORT |= (1<<m1_2);m1_PORT &= ~(1<<m1_1)

#define m2_DDR				DDRA
#define m2_PORT				PORTA
#define m2_1				3
#define m2_2				2
#define m2_stall			m2_PORT |= (1<<m2_1)|(1<<m2_2)
#define m2_rev				m2_PORT |= (1<<m2_1);m2_PORT &= ~(1<<m2_2)
#define m2_fwd				m2_PORT |= (1<<m2_2);m2_PORT &= ~(1<<m2_1)

#define m3_DDR				DDRA
#define m3_PORT				PORTA
#define m3_1				6
#define m3_2				7
#define m3_stall			m3_PORT |= (1<<m3_1)|(1<<m3_2)
#define m3_rev				m3_PORT |= (1<<m3_1);m3_PORT &= ~(1<<m3_2)
#define m3_fwd				m3_PORT |= (1<<m3_2);m3_PORT &= ~(1<<m3_1)

#define m4_DDR				DDRA
#define m4_PORT				PORTA
#define m4_1				4
#define m4_2				5
#define m4_stall			m4_PORT |= (1<<m4_1)|(1<<m4_2)
#define m4_rev				m4_PORT |= (1<<m4_1);m4_PORT &= ~(1<<m4_2)
#define m4_fwd				m4_PORT |= (1<<m4_2);m4_PORT &= ~(1<<m4_1)

#define m5_DDR				DDRH
#define m5_PORT				PORTH
#define m5_1				4
#define m5_2				5
#define m5_stall			m5_PORT |= (1<<m5_1)|(1<<m5_2)
#define m5_rev				m5_PORT |= (1<<m5_1);m5_PORT &= ~(1<<m5_2)
#define m5_fwd				m5_PORT |= (1<<m5_2);m5_PORT &= ~(1<<m5_1)

#define m6_DDR				DDRH
#define m6_PORT				PORTH
#define m6_1				7
#define m6_2				6
#define m6_stall			m6_PORT |= (1<<m6_1)|(1<<m6_2)
#define m6_rev				m6_PORT |= (1<<m6_1);m6_PORT &= ~(1<<m6_2)
#define m6_fwd				m6_PORT |= (1<<m6_2);m6_PORT &= ~(1<<m6_1)

#define flash_DDR           DDRG
#define flash_PORT          PORTG
#define flashOn             flash_PORT = (1<<1);
#define flashOff            flash_PORT = 0;

/*--------------------- GLOBAL VARIABLES -----------------------*/
volatile int8_t ticks[NUMBER_OF_MOTORS];		//Initially the ticks are considered to be zero
int8_t rotate_dir[NUMBER_OF_MOTORS];
int8_t stop_tick[NUMBER_OF_MOTORS];
volatile int8_t tick_reached[NUMBER_OF_MOTORS];

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

ISR(PCINT2_vect)
{
	int8_t i;	
	if (TCNT1 > 4800)
	{
		for(i=0; i<6; i++)
		{
			if(PINK & (1<<i))
			{
				switch (i)
				{
					case 0 : m1_stall;break;
					case 1 : m2_stall;break;
					case 2 : m3_stall;break;
					case 3 : m4_stall;break;
					case 4 : m5_stall;break;
					case 5 : m6_stall;break;
					default:;
				}
			}
		}
	}
}
ISR(BADISR_vect)
{
	return;
}

ISR(TIMER1_COMPA_vect)
{
	for(int8_t i=0;i<NUMBER_OF_MOTORS;i++)
		tick_reached[i] = 1;
}
void reach_to_ticks(int8_t final_ticks, int8_t motor_number, uint8_t rotation_dir)
{
	stop_tick[motor_number] = final_ticks;
	rotate_dir[motor_number] = rotation_dir;
	tick_reached[motor_number] = 0;
	//This function takes input the number of ticks and dynamically rotates the motor in the given direction until it reaches the final ticks
	if(final_ticks != ticks[motor_number])
	{
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
}
void init_lcds()
{
	//Initialize the LCD
	lcd_port_config();
	lcd_set_4bit();
	lcd_init();
}

void init_flash()
{
	flash_DDR = 0xFF;
	flashOff;
}

void init_motors()
{
	//Declare the motor input pins as output and keep the input Pins ready
	m1_DDR |= (1<<m1_1)|(1<<m1_2);
	m2_DDR |= (1<<m2_1)|(1<<m2_2);
	m3_DDR |= (1<<m3_1)|(1<<m3_2);
	m4_DDR |= (1<<m4_1)|(1<<m4_2);
	m5_DDR |= (1<<m5_1)|(1<<m5_2);
	m6_DDR |= (1<<m6_1)|(1<<m6_2);
	//Stall all the motors
}
void init_external_interrupts()
{
	//Interrupt pins for our use will be PD 0,1,2,3 and PE 4,5
	//All the interrupt pins are declared as INPUT PINS
	DDRK &= (~(1<<0))&(~(1<<1))&(~(1<<2))&(~(1<<3))&(~(1<<4))&(~(1<<5));
	PORTK |= (1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5);
	//Initialize all the interrupts
	
	//Disable global interrupt //SREG -> I-bit set to 1
	cli();
	//Enable the Interrupt Request
	PCMSK2 = (1<<PCINT21)|(1<<PCINT20)|(1<<PCINT19)|(1<<PCINT18)|(1<<PCINT17)|(1<<PCINT16);
	//Set the Interrupt type
	
	PCICR = (1<<PCIE2);
// 	//Enable global interrupt
	sei();
}
void init_timer_interrupt()
{
	TCCR1A = 0;
	TCCR1B = (1<<WGM12)|(1<<CS12)|(0<<CS11)|(1<<CS10);    //CTC mode Pre-scaler 1024
	TIMSK1 = (1<<OCIE1A);
	OCR1A = 28800;  //Time 2seconds
}

void init()
{
	int8_t i;
	init_lcds();
	init_motors();
	init_flash();
	init_external_interrupts();
	init_timer_interrupt();
	
	//Initialize all the ticks and rotation direction to be zeros
	for(i=0;i<NUMBER_OF_MOTORS;i++)
		ticks[i] = rotate_dir[i] = stop_tick[i] = tick_reached[i] = 0;
}
void motor_test()
{
	m1_fwd;
	_delay_ms(1000);
	m2_fwd;
	_delay_ms(1000);
	m3_fwd;
	_delay_ms(1000);
	m4_fwd;
	_delay_ms(1000);
	m5_fwd;
	_delay_ms(1000);
	m6_fwd;
	_delay_ms(1000);
}
void encoder_test()
{
	m1_fwd;
	while((tick_reached[m1]==0));
	tick_reached[m1] = 0;
	m2_fwd;
	while((tick_reached[m2]==0));
	tick_reached[m2] = 0;
	m3_fwd;
	while((tick_reached[m3]==0));
	tick_reached[m3] = 0;
	m4_fwd;
	while((tick_reached[m4]==0));
	tick_reached[m4] = 0;
	m5_fwd;
	while((tick_reached[m5]==0));
	tick_reached[m5] = 0;
	m6_fwd;
	while((tick_reached[m6]==0));
	tick_reached[m6] = 0;
}
/*---------------------------- TOUCH SCREEN HELPER FUNCTIONS -------------------------*/
void flash_on()
{
	flashOn;
	//USART1_Transmit_8Bit('a');
	
}
void flash_off()
{
	flashOff;
}
void ground_left()
{
	TCNT1 = 0;
	m1_fwd;
	m3_rev;
	m5_fwd;
	tick_reached[m1] = 0;
	tick_reached[m3] = 0;
	tick_reached[m5] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m3]==0) || (tick_reached[m5]==0));
	
	TCNT1 = 0;
	m2_rev;
	m4_fwd;
	m6_rev;
	tick_reached[m2] = 0;
	tick_reached[m4] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m2]==0) || (tick_reached[m4]==0) || (tick_reached[m6]==0));
	
	//USART1_Transmit_8Bit('c');
	//usart_data = 0;
}
void ground_right()
{
	TCNT1 = 0;
	m1_rev;
	m3_fwd;
	m5_rev;
	tick_reached[m1] = 0;
	tick_reached[m3] = 0;
	tick_reached[m5] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m3]==0) || (tick_reached[m5]==0));
	
	TCNT1 = 0;
	m2_fwd;
	m4_rev;
	m6_fwd;
	tick_reached[m2] = 0;
	tick_reached[m4] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m2]==0) || (tick_reached[m4]==0) || (tick_reached[m6]==0));
	
	//USART1_Transmit_8Bit('d');
	//usart_data = 0;
}
void ground_front()
{
	TCNT1 = 0;
	m1_fwd;
	m3_fwd;
	m5_fwd;
	tick_reached[m1] = 0;
	tick_reached[m3] = 0;
	tick_reached[m5] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m3]==0) || (tick_reached[m5]==0));
	
	TCNT1 = 0;
	m2_fwd;
	m4_fwd;
	m6_fwd;
	tick_reached[m2] = 0;
	tick_reached[m4] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m2]==0) || (tick_reached[m4]==0) || (tick_reached[m6]==0));	
	//USART1_Transmit_8Bit('e');
	//usart_data = 0;
}
void ground_reset()
{
	m1_stall;
	m2_stall;
	m3_stall;
	m4_stall;
	m5_stall;
	m6_stall;
	//USART1_Transmit_8Bit('f');
	//usart_data = 0;
}
void stairs_left()
{
	TCNT1 = 0;
	m1_fwd;
	m2_rev;
	tick_reached[m1] = 0;
	tick_reached[m2] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m2]==0));
	
	TCNT1 = 0;
	m5_fwd;
	m6_rev;
	tick_reached[m5] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m5]==0) || (tick_reached[m6]==0));
	
	TCNT1 = 0;
	m3_rev;
	m4_fwd;
	tick_reached[m3] = 0;
	tick_reached[m4] = 0;
	while((tick_reached[m3]==0) || (tick_reached[m4]==0));
	//USART1_Transmit_8Bit('g');
	//usart_data = 0;
}
void stairs_right()
{
	TCNT1 = 0;
	m1_rev;
	m2_fwd;
	tick_reached[m1] = 0;
	tick_reached[m2] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m2]==0));
	
	TCNT1 = 0;
	m5_rev;
	m6_fwd;
	tick_reached[m5] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m5]==0) || (tick_reached[m6]==0));
	
	TCNT1 = 0;
	m3_fwd;
	m4_rev;
	tick_reached[m3] = 0;
	tick_reached[m4] = 0;
	while((tick_reached[m3]==0) || (tick_reached[m4]==0));
	//USART1_Transmit_8Bit('h');
	//usart_data = 0;
}
void stairs_front()
{
	TCNT1 = 0;
	m1_fwd;
	m2_fwd;
	tick_reached[m1] = 0;
	tick_reached[m2] = 0;
	while((tick_reached[m1]==0) || (tick_reached[m2]==0));
	
	TCNT1 = 0;
	m5_fwd;
	m6_fwd;
	tick_reached[m5] = 0;
	tick_reached[m6] = 0;
	while((tick_reached[m5]==0) || (tick_reached[m6]==0));
	
	TCNT1 = 0;
	m3_fwd;
	m4_fwd;
	tick_reached[m3] = 0;
	tick_reached[m4] = 0;
	while((tick_reached[m3]==0) || (tick_reached[m4]==0));
	//USART1_Transmit_8Bit('i');
	//usart_data = 0;
}
void stairs_reset()
{
	m1_stall;
	m2_stall;
	m3_stall;
	m4_stall;
	m5_stall;
	m6_stall;
	//USART1_Transmit_8Bit('j');
	//usart_data = 0;
}

void initialize()
{
	TCNT1 = 0;
	m1_fwd;
	m2_fwd;
	m5_fwd;
	m6_fwd;
	m3_fwd;
	m4_fwd;
	
	tick_reached[m1] = 0;
	tick_reached[m2] = 0;
	tick_reached[m5] = 0;
	tick_reached[m6] = 0;
	tick_reached[m3] = 0;
	tick_reached[m4] = 0;
	
	while((tick_reached[m1]==0) || (tick_reached[m2]==0)||(tick_reached[m5]==0) || (tick_reached[m6]==0)||(tick_reached[m3]==0) || (tick_reached[m4]==0));	
}
/*---------------------------- TOUCH SCREEN HELPER FUNCTIONS ENDS HERE -------------------------*/

int main(void)
{
	_delay_ms(2000);
	init();
	initialize();
	/* CONVENSION TO RUN THE MOTORS
	=> Set the rotation direction
	=> call the function reach_to_ticks with "final_ticks" and "motor_number" as arguments*/
 	
	while(1)
 	 {
		ground_front();
	 }
	return 0;
}