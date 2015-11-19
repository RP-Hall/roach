
#ifndef USART2560_H_
#define USART2560_H_


//------------------------------USART1------------------------------//
void USART1_Init( unsigned int ubrr);
void USART1_Transmit_8Bit( unsigned char data );
unsigned char USART1_Receive_8Bit( void );
//------------------------------USART1------------------------------//
//******************************************************************//

extern volatile int8_t usart_data;

#endif /* USART2560_H_ */