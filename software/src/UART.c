/**
 * UART.c
 *
 * Created: 5/26/2018 1:08:16 PM
 * Author: Patrick Dunham
 */

#include "UART.h"


/*** PUBLIC FUNCTIONS ***/

/** @brief Initialize UART to UART_BAUD at 8-N-1 w/ TX and RX.
 *
 * This will enable TX and RX interrupts to allow for non-blocking
 * communication. The send function will return immediatley, and the recieve
 * function will return whatever's in the buffer, whether or not it is an
 * actual character. Check the UART control variables to be safe.
 *
 * NOTE: Any previous UART configuration will be overwritten.
 * NOTE: Ensure F_CPU is set to the designated clock frequency, either in
 *       UART.h or elsewhere in the program.
 *
 * @param _baud Baud rate to configure UART for [bps]
 */
void urt_init(uint16_t _baud) {
  // Set baud rate
	UBRR0L = (F_CPU / (16UL * _baud)) - 1;

	// Enable tx/rx & interrupts
	UCSR0B = (1<<TXCIE0)|(1<<RXCIE0)|(1<<TXEN0)|(1<<RXEN0);
}

/** @brief Recieves a character from the UART buffer.
 *
 * Will return immediatly with the buffer contents.
 *
 * @returns The raw byte received over UART
 */
uint8_t urt_recv(void) {
  return UDR0;
}

/** @brief Recieves a character from the UART buffer, blocking for valid data.
 *
 * Will return when a character has been received over UART
 *
 * @returns The raw byte received over UART
 */
uint8_t urt_recv_block(void) {
  // Wait to receive
	while(!(UCSR0A & (1<<RXC0))) { }

  return UDR0;
}

/** @brief Recieves a character from the UART buffer, blocking for valid data.
 *
 * Will return when a character has been received over UART
 *
 * @param _timeout Max time to wait for new character, in arbitrary units
 *
 * @returns The raw byte received over UART
 */
uint8_t urt_recv_timeout(uint32_t _timeout) {
  // Wait to receive
  do {
		// Check for reception
		if((UCSR0A & (1<<RXC0))) {
			_timeout = 0;
		}

	} while(_timeout--);

  return UDR0;
}

/** @brief Queues a single character to be sent over UART.
 *
 * Will return immediatly.
 *
 * @param _data The raw byte to send over UART
 */
void urt_send(uint8_t _data) {
  UDR0 = _data;
}

/** @brief Sends a character over UART, blocking until it is sent.
 *
 * Will return when the character is sent
 *
 * @param _data The raw byte to send over UART
 */
void urt_send_block(uint8_t _data) {
  UDR0 = _data;

  // Wait to send
	while(!(UCSR0A & (1<<UDRE0))) {	}
}


/*** PRIVATE FUNCTIONS ***/
