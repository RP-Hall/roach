

#ifndef INIT_H_
#define INIT_H_

void init_lcds(void);
void init_motors(void);
void init_external_interrupts(void);
void init_timer_interrupt();
void init(void);
void init_flash();

/*
rotate = 1 : rev
rotate = 0 : fwd

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

#define flash_DDR           DDRG
#define flash_PORT          PORTG
#define flashOn1             flash_PORT |= (1<<1);
#define flashOff1            flash_PORT &= ~(1<<1);
#define flashOn2             flash_PORT |= (1<<0);
#define flashOff2            flash_PORT &= ~(1<<0);

#define m1_offset 5
#define m2_offset 3
#define m3_offset 4
#define m4_offset 4
#define m5_offset 5
#define m6_offset 5

#define m1_DDR				DDRH
#define m1_PORT				PORTH
#define m1_1				4
#define m1_2				6
#define m1_stall			m1_PORT |= (1<<m1_1)|(1<<m1_2)
#define m1_rev				m1_PORT |= (1<<m1_1);m1_PORT &= ~(1<<m1_2)
#define m1_fwd				m1_PORT |= (1<<m1_2);m1_PORT &= ~(1<<m1_1)

#define m2_DDR				DDRH
#define m2_PORT				PORTH
#define m2_1				0
#define m2_2				2
#define m2_stall			m2_PORT |= (1<<m2_1)|(1<<m2_2)
#define m2_rev				m2_PORT |= (1<<m2_1);m2_PORT &= ~(1<<m2_2)
#define m2_fwd				m2_PORT |= (1<<m2_2);m2_PORT &= ~(1<<m2_1)

#define m3_DDR				DDRH
#define m3_PORT				PORTH
#define m3_1				1
#define m3_2				3
#define m3_stall			m3_PORT |= (1<<m3_1)|(1<<m3_2)
#define m3_rev				m3_PORT |= (1<<m3_1);m3_PORT &= ~(1<<m3_2)
#define m3_fwd				m3_PORT |= (1<<m3_2);m3_PORT &= ~(1<<m3_1)

#define m4_DDR				DDRH
#define m4_PORT				PORTH
#define m4_1				7
#define m4_2				5
#define m4_stall			m4_PORT |= (1<<m4_1)|(1<<m4_2)
#define m4_rev				m4_PORT |= (1<<m4_1);m4_PORT &= ~(1<<m4_2)
#define m4_fwd				m4_PORT |= (1<<m4_2);m4_PORT &= ~(1<<m4_1)

#define m5_DDR				DDRB
#define m5_PORT				PORTB
#define m5_1				5
#define m5_2				7
#define m5_stall			m5_PORT |= (1<<m5_1)|(1<<m5_2)
#define m5_rev				m5_PORT |= (1<<m5_1);m5_PORT &= ~(1<<m5_2)
#define m5_fwd				m5_PORT |= (1<<m5_2);m5_PORT &= ~(1<<m5_1)

#define m6_DDR				DDRB
#define m6_PORT				PORTB
#define m6_1				3
#define m6_2				1
#define m6_stall			m6_PORT |= (1<<m6_1)|(1<<m6_2)
#define m6_rev				m6_PORT |= (1<<m6_1);m6_PORT &= ~(1<<m6_2)
#define m6_fwd				m6_PORT |= (1<<m6_2);m6_PORT &= ~(1<<m6_1)

/*--------------------- GLOBAL VARIABLES -----------------------*/
volatile int8_t ticks[NUMBER_OF_MOTORS];		//Initially the ticks are considered to be zero
int8_t rotate_dir[NUMBER_OF_MOTORS];
int8_t stop_tick[NUMBER_OF_MOTORS];
volatile int8_t tick_reached[NUMBER_OF_MOTORS];
volatile int8_t motor_initialized[NUMBER_OF_MOTORS];
volatile int8_t updated_ticks[NUMBER_OF_MOTORS];
volatile int8_t motor_offset_flag[NUMBER_OF_MOTORS];
volatile int8_t stall_flag;




#endif /* INIT_H_ */