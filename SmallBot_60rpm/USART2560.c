
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "USART2560.h"

/* ATMEGA2560 contains USART0 , USART1 , USART2 , USART3

   USART0   :   TXD0        -         PE1 (PORTE PIN1) 
                RXD0        -         PE0 (PORTE PIN0)

   USART1   :   TXD1        -         PD3 (PORTD PIN3)
                RXD1        -         PD2 (PORTE PIN2)

   USART2   :   TXD2        -         PH1 (PORTH PIN1)
                RXD0        -         PH0 (PORTH PIN0)

   USART3   :   TXD3        -         PJ1 (PORTJ PIN1)
                RXD3        -         PJ0 (PORTJ PIN0)

*/
 
volatile int8_t usart_data;

//------------------------------USART1------------------------------//
void USART1_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRR3H = (unsigned char)(ubrr>>8);
	UBRR3L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR3B = (1<<RXEN3)|(1<<TXEN3)|(1<<RXCIE3)|(1<<TXCIE3);
	/* Set frame format: 8data, 2stop bit */
	UCSR3C = (1<<USBS3)|(3<<UCSZ30);
}

void USART1_Transmit_8Bit( unsigned char data )
{
	/* Wait for empty transmit buffer */
	while ( !( UCSR3A & (1<<UDRE3)) )
	;
	/* Put data into buffer, sends the data */
	UDR3 = data;
}

unsigned char USART1_Receive_8Bit( void )
{
	/* Wait for data to be received */
	while ( !(UCSR3A & (1<<RXC3)) )
	;
	/* Get and return received data from buffer */
	return UDR3;
}


ISR (USART3_RX_vect)
{
	usart_data = UDR3;
	//lcd_print_int(1,1,usart_data);
}