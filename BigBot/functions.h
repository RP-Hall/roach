#include "init.h"
#include "USART2560.h"
//#include "USART2560.c"

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

void reach_to_ticks(int8_t final_ticks, int8_t motor_number, uint8_t rotation_dir);
/*--------------------- LCD Helper Function --------------------------------*/

void lcd_print_int(int8_t line_no, int8_t pos_no, int8_t number);
void print_all_ticks(void);

void initialize_motors();
void motor_test();
void encoder_test();
void move_forward_limp();
void flash_on();
void flash_off();
void ground_left();
void ground_right();
void ground_front();
void ground_reset();
void stairs_left();
void stairs_right();
void stairs_front();
void stairs_reset();
void ground_reverse();
void stairs_reverse();
void leaping();
void reset_ticks();




#endif /* FUNCTIONS_H_ */