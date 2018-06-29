/** @file UART.h
 *  @brief Driver for ATMEGA328P UART.
 *
 *  Minimal UART driver for the above. Not compatible with `fprintf()`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdint.h>


/* --CONFIGURATION-- */

#ifndef F_CPU
  #define F_CPU 16000000UL
#endif /* F_CPU */


/* -- PUBLIC FUNCTIONS -- */

/** @brief Initialize UART to _baud at 8-N-1 w/ TX and RX.
 *
 *  This will enable TX and RX interrupts to allow for non-blocking
 *  communication. The send function will return immediatley, and the recieve
 *  function will return whatever's in the buffer, whether or not it is an
 *  actual character. Check the UART control variables to be safe.
 *
 *  NOTE: Any previous UART configuration will be overwritten.
 *  NOTE: Ensure F_CPU is set to the designated clock frequency, either in
 *        UART.h or elsewhere in the program.
 *
 *  @param _baud Baud rate to configure UART for [bps]
 *  @returns Void.
 */
void urt_init(uint16_t _baud);

/** @brief Recieves a character from the UART buffer.
 *
 *  Will return immediatly with the buffer contents.
 *
 *  @returns The raw byte received over UART
 */
uint8_t urt_recv(void);

/** @brief Recieves a character from the UART buffer, blocking for valid data.
 *
 *  Will return when a character has been received over UART
 *
 *  @returns The raw byte received over UART
 */
uint8_t urt_recv_block(void);

/** @brief Recieves a character from the UART buffer, blocking w/ timeout.
 *
 *  Will return when a character has been received over UART, or when the
 *  timeout expires. The timeout is expressed as the number of loop iterations
 *  in the function below. Experimentation is needed to determine a real-world
 *  time equivalent.
 *
 *  @param _timeout Max time to wait for new character, in arbitrary units
 *  @returns The raw byte received over UART
 */
uint8_t urt_recv_timeout(uint32_t _timeout);

/** @brief Queues a single character to be sent over UART.
 *
 *  Will return immediatly.
 *
 *  @param _data The raw byte to send over UART
 *  @returns Void.
 */
void urt_send(uint8_t _data);

/** @brief Sends a character over UART, blocking until it is sent.
 *
 *  Will return when the character is sent
 *
 *  @param _data The raw byte to send over UART
 *  @returns Void.
 */
void urt_send_block(uint8_t _data);


#endif /* UART_H */
