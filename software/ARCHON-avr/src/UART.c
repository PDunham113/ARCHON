/** @file UART.c
 *  @brief Driver for ATMEGA328P UART.
 *
 *  This contains the implementation for the interface described in `UART.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#include "UART.h"


/* -- PUBLIC FUNCTIONS -- */

void urt_init(uint16_t _baud) {
  // Set baud rate
	UBRR0L = (F_CPU / (16UL * _baud)) - 1;

	// Enable tx/rx & interrupts
	UCSR0B = (1 << TXCIE0) | (1 << RXCIE0) | (1 << TXEN0) | (1 <<  RXEN0);
}

uint8_t urt_recv(void) {
  return UDR0;
}

uint8_t urt_recv_block(void) {
  // Wait to receive
	while(!(UCSR0A & (1 << RXC0))) { }

  return UDR0;
}

uint8_t urt_recv_timeout(uint32_t _timeout) {
  // Wait to receive
  do {
		// Check for reception
		if((UCSR0A & (1 << RXC0))) {
			_timeout = 0;
		}

	} while(_timeout--);

  return UDR0;
}

void urt_send(uint8_t _data) {
  UDR0 = _data;
}

void urt_send_block(uint8_t _data) {
  UDR0 = _data;

  // Wait to send
	while(!(UCSR0A & (1 << UDRE0))) {	}
}
