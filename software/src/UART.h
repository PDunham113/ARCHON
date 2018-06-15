/**
 * UART.h
 *
 * Minimal UART driver for ATMega328p
 *
 * Created: 5/26/2018 1:08:16 PM
 * Author: Patrick Dunham
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdint.h>


/*** CONFIGURATION ***/

#ifndef F_CPU
#define F_CPU 16000000UL
#endif


/*** VARIABLES & DEFINITIONS ***/


/*** FUNCTIONS ***/

// Public
void urt_init(uint16_t _baud);

uint8_t urt_recv(void);
uint8_t urt_recv_block(void);
uint8_t urt_recv_timeout(uint32_t _timeout);
void urt_send(uint8_t);
void urt_send_block(uint8_t);

// Private

#endif /* UART_H_ */
