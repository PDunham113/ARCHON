/**
 * COMM.h
 *
 * Created: 5/21/2018 9:42:48 PM
 * Author: Patrick Dunham
 */

#ifndef COMM_H_
#define COMM_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "UART.h"


/*** CONFIGURATION ***/

#define COM_MAX_DATA_LENGTH  16
#define COM_RECV()           urt_recv()
#define COM_SEND(_byte)      urt_send(_byte)
#define COM_RX_INTERRUPT
#define COM_TX_INTERRUPT


/*** VARIABLES & DEFINITIONS ***/

// Control Characters
#define SOM           0x01
#define ACK           0x06
#define NACK          0x15

// Errors
#define NO_ERROR      0x00
#define COM_ERROR_EC  0x01

// COMM RX States
#define COM_RX_INIT   0x00
#define COM_RX_TYPE   0x01
#define COM_RX_LENGTH 0x02
#define COM_RX_DATA   0x03
#define COM_RX_EC     0x04
#define COM_RX_DONE   0x05

// COMM TX States
#define COM_TX_INIT   0x00
#define COM_TX_TYPE   0x01
#define COM_TX_LENGTH 0x02
#define COM_TX_DATA   0x03
#define COM_TX_EC     0x04
#define COM_TX_POST   0x05
#define COM_TX_DONE   0x06

// Message Types
#define COM_PKT_EMPTY 0x00
#define COM_PKT_TEST  0x01

// Status Bits
#define COM_TX_READY     0
#define COM_RX_READY     1
#define COM_TX_BUSY      2
#define COM_RX_BUSY      3
#define COM_TX_ERROR     4
#define COM_RX_ERROR     5

// Message Structure
typedef struct COM_Message_ {
  uint8_t type;
  uint8_t length;
  uint8_t data[COM_MAX_DATA_LENGTH];
  uint8_t checksum;
  uint8_t state;
} COM_Message;

// Status Variables
static volatile COM_Message com_rx_buffer;
static volatile COM_Message com_tx_buffer;


/** COMM Message Buffer Status
 * [RES]|[RES]|[RX_ERROR]|[TX_ERROR]|[RX_BUSY]|[TX_BUSY]|[RX_READY]|[TX_READY]
 *
 * RES      - Reserved for future usage.
 * TX_READY - Set to 1 when message has completed transmitting. Indicates state
 *            machine is ready for another message to be sent.
 * RX_READY - Set to 1 when message has been received. Indicates message is
 *            ready to be read. Must be set to 0 when message has been used.
 *            Otherwise, messages will be ignored.
 * TX_BUSY  - Set to 1 while message is being transmitted. Set to 0 when
 *            message is not being transmitted.
 * RX_BUSY  - Set to 1 while message is being received. Set to 0 when message
 *            is not being received.
 * TX_ERROR - Set to 1 when error occurs during transmission. Must be set to 0
 *            once error is dealt with.  Check com_tx_error for more
 *            information.
 * RX_ERROR - Set to 1 when error occurs during reception. Must be set to 0
 *            once error is dealt with.  Check com_rx_error for more
 *            information.
 */
extern volatile uint8_t com_status;
extern volatile uint8_t com_rx_error;
extern volatile uint8_t com_tx_error;


/*** FUNCTIONS ***/

// Public
void com_send_packet(COM_Message _message);
COM_Message com_rec_packet(void);

// Private
void com_send_state_machine(void);
void com_rec_state_machine(void);

uint8_t com_calc_checksum(COM_Message* _message);

#endif /* COMM_H_ */
