/** @file COMM.h
 *  @brief Simple packet-based communications layer.
 *
 *  This contains the interface and documentation for the above-mentioned
 *  communications protocol. Packets are structured as follows:
 *  |--SOM--| |--byte--| |---byte---| |-(MSG_LENGTH) bytes-| |-byte-|
 *  [MSG_SOM]|[MSG_TYPE]|[MSG_LENGTH]|[------MSG_DATA------]|[MSG_EC]
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 */

#ifndef COMM_H
#define COMM_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#include "UART.h"


/*** CONFIGURATION ***/

#define COM_MAX_DATA_LENGTH 16
#define COM_RECV()          urt_recv()
#define COM_SEND(_byte)     urt_send(_byte)
#define COM_RX_INTERRUPT()  ISR(USART_RX_vect)
#define COM_TX_INTERRUPT()  ISR(USART_TX_vect)


/*** VARIABLES & DEFINITIONS ***/

// Control Characters
#define SOM           0x01
#define ACK           0x06
#define NACK          0x15

// Errors
typedef enum com_error {
  COM_NO_ERROR,
  COM_ERROR_EC,
} com_error_t;

// COMM RX & TX States
typedef enum com_state {
  COM_INIT,
  COM_TYPE,
  COM_LENGTH,
  COM_DATA,
  COM_EC,
  COM_POST,
  COM_DONE,
} com_state_t;

// Message Types
typedef enum com_type {
  COM_PKT_EMPTY,
  COM_PKT_TEST,
} com_type_t;

// Status Bits
#define COM_TX_READY     0
#define COM_RX_READY     1
#define COM_TX_BUSY      2
#define COM_RX_BUSY      3
#define COM_TX_ERROR     4
#define COM_RX_ERROR     5

// Message Structure
typedef struct com_message {
  com_type_t type;
  uint8_t length;
  uint8_t data[COM_MAX_DATA_LENGTH];
  uint8_t checksum;
  com_state_t state;
} com_message_t;

// Status Variables
static volatile com_message_t com_rx_buffer;
static volatile com_message_t com_tx_buffer;


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
 * NOTE: *_BUSY and *_READY flags are handled automatically by
 *       `com_send_packet` and `com_rec_packet`.
 */
extern volatile uint8_t com_status;
extern volatile com_error_t com_rx_error;
extern volatile com_error_t com_tx_error;


/* -- PUBLIC FUNCTIONS -- */

/** @brief Starts sending a COM packet over UART
 *
 *  Packet is added to internal buffer and state machine is initialized.
 *  Anything currently in the buffer is overwritten. To ensure there are no
 *  packets currently being sent, check the status of the `TX_READY` flag.
 *
 *  @param _message Message packet to send
 *  @returns Void.
 */
void com_send_packet(com_message_t _message);

/** @brief Gets the COM packet stored in the buffer
 *
 *  Packet is pulled from internal buffer and state machine is reset. Can be
 *  used to pull partial packets. To ensure access of a full packet, check the
 *  status of the `RX_READY` flag.
 *
 *  @returns Message packet recieved
 */
com_message_t com_rec_packet(void);


/* -- PRIVATE FUNCTIONS -- */

/** @brief Continues sending a COM packet over UART
 *
 *  Proceeds through the following states, in order:
 *  - COM_INIT:   Initial state of message - preprocessing & initialize hardware
 *  - COM TYPE:   Send message type
 *  - COM_LENGTH: Send message length, reset COM_DATA state variable
 *  - COM_DATA:   Send each data byte in the message
 *  - COM_EC:     Send message checksum
 *  - COM_POST:   postprocessing, indicate message is sent
 *  - COM_DONE:   Final state of message
 *
 *  Called once in `com_send_packet()`, otherwise called through the COMM TX
 *  ISR.
 *  @returns Void.
 */
static void com_send_state_machine(void);

/** @brief Receives a COM packet over UART
 *
 *  Proceeds through the following states, in order:
 *  - COM_INIT:   Initial state of message - initialize hardware
 *  - COM TYPE:   Receive message type
 *  - COM_LENGTH: Receive message length, reset COM_DATA state variable
 *  - COM_DATA:   Receive each data byte in the message
 *  - COM_EC:     Receive & verify message checksum
 *  - COM_DONE:   Final state of message
 *
 *  @returns Void.
 */
static void com_rec_state_machine(void);

/** @brief Calculates checksum byte for messages
 *
 *  Currently a simple, ineffective XOR placeholder. Each byte in the message,
 *  except for the checksum, is XORED together.
 *
 *  @param _message Message to calculate checksum byte for
 *  @returns Calculated checksum byte
 */
static uint8_t com_calc_checksum(volatile com_message_t *_message);


/* -- ISRS -- */

/** @brief COMM library RX interrupt
 *
 *  Exact function call can be configured in the CONFIGURATION section. Ideally
 *  is set to the RX ISR for whatever hardware layer this library is using.
 *  Failing that, this can be any function prototype of the form
 *  `void foo(void)` that is called after receiving a character.
 *
 *  NOTE: Ensure that whatever function is used here, ISR or otherwise, is
 *        *NOT* declared elsewhere.
 *
 *  @returns Void.
 */
COM_RX_INTERRUPT();

/** @brief COMM library TX interrupt
 *
 *  Exact function call can be configured in the CONFIGURATION section. Ideally
 *  is set to the TX ISR for whatever hardware layer this library is using.
 *  Failing that, this can be any function prototype of the form
 *  `void foo(void)` that is called after finishing sending a character.
 *
 *  NOTE: Ensure that whatever function is used here, ISR or otherwise, is
 *        *NOT* declared elsewhere.
 *
 *  @returns Void.
 */
COM_TX_INTERRUPT();


#endif /* COMM_H */
