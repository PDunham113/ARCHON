/**
 * COMM.c
 *
 * Created: 5/21/2018 9:42:48 PM
 * Author: Patrick Dunham
 */

#include "COMM.h"

volatile uint8_t com_status = (1 << COM_TX_READY);
volatile uint8_t com_rx_error = 0;
volatile uint8_t com_tx_error = 0;

/*** PUBLIC FUNCTIONS ***/

/** @brief Starts sending a COM packet over UART
 *
 * Packet is added to internal buffer and state machine is initialized.
 * Anything currently in the buffer is overwritten.
 *
 * @param _message Message packet to send
 */
void com_send_packet(COM_Message _message) {
  if (com_status & (1 << COM_TX_READY)) {
    com_tx_buffer = _message;

    com_tx_buffer.state = COM_TX_INIT;

    com_status &= ~(1 << COM_TX_READY);

    com_send_state_machine();
  }
}


/** @brief Gets the COM packet stored in the buffer
 *
 * Packet is pulled from internal buffer and state machine is reset. Can be
 * used to pull partial packets.
 *
 * @returns Message packet recieved
 */
COM_Message com_rec_packet(void) {
  com_status &= ~(1 << COM_RX_READY);

  return com_rx_buffer;
}


/*** PRIVATE FUNCTIONS ***/

/** @brief Continues sending a COM packet over UART
 *
 */
void com_send_state_machine(void) {
  static uint8_t _data_loc;

  /* STATE MACHINE */

  switch (com_tx_buffer.state) {
    case COM_TX_INIT: {
      // Tell the world we're busy
      com_status |= (1 << COM_TX_BUSY);

      // Inspect packets to ensure they match
      switch (com_tx_buffer.type) {
        // Empty packets are empty
        case COM_PKT_EMPTY: {
          com_tx_buffer.length = 0;
        } break;

        // Test packets are flexible
        case COM_PKT_TEST: {
        } break;

        default: {
        } break;
      }

      com_tx_buffer.checksum = com_calc_checksum(&com_tx_buffer);
      COM_SEND(SOM);

      com_tx_buffer.state = COM_TX_TYPE;
    } break;

    case COM_TX_TYPE: {
      COM_SEND(com_tx_buffer.type);

      com_tx_buffer.state = COM_TX_LENGTH;
    } break;

    case COM_TX_LENGTH: {
      COM_SEND(com_tx_buffer.length);

      _data_loc = 0;
      com_tx_buffer.state = COM_TX_DATA;
    } break;

    case COM_TX_DATA: {
      COM_SEND(com_tx_buffer.data[_data_loc]);
      _data_loc++;

      if (_data_loc >= com_tx_buffer.length
          || _data_loc >= COM_MAX_DATA_LENGTH) {
        com_tx_buffer.state = COM_TX_EC;
      }
    } break;

    case COM_TX_EC: {
      COM_SEND(com_tx_buffer.checksum);

      com_tx_buffer.state = COM_TX_POST;
    } break;

    case COM_TX_POST: {
      // Tell the world we're done
      com_status &= ~(1 << COM_TX_BUSY);
      com_status |= (1 << COM_TX_READY);

      com_tx_buffer.state = COM_TX_DONE;
    } break;

    default: {

    } break;
  }
}

/** @brief Receives a COM packet over UART
 *
 * Called after receiving a SOM
 */
void com_rec_state_machine(void) {
  static uint8_t _data_loc;

  switch (com_rx_buffer.state) {
    case COM_RX_INIT: {
      // Tell the world we're busy
      com_status |= (1 << COM_RX_BUSY);

      com_rx_buffer.state = COM_RX_TYPE;
    } break;

    case COM_RX_TYPE: {
      com_rx_buffer.type = COM_RECV();

      com_rx_buffer.state = COM_RX_LENGTH;
    } break;

    case COM_RX_LENGTH: {
      com_rx_buffer.length = COM_RECV();

      _data_loc = 0;
      com_rx_buffer.state = COM_RX_DATA;
    } break;

    case COM_RX_DATA: {
      com_rx_buffer.data[_data_loc] = COM_RECV();
      _data_loc++;

      if (_data_loc >= com_rx_buffer.length
          || _data_loc >= COM_MAX_DATA_LENGTH) {
        com_rx_buffer.state = COM_RX_EC;
      }
    } break;

    case COM_RX_EC: {
      com_rx_buffer.checksum = COM_RECV();

      uint8_t _checksum = com_calc_checksum(&com_rx_buffer);

      if (_checksum != com_rx_buffer.checksum) {
        com_status |= (1 << COM_RX_ERROR);
        com_rx_error = COM_ERROR_EC;
      }

      com_status &= ~(1 << COM_RX_BUSY);
      com_status |= (1 << COM_RX_READY);

      com_rx_buffer.state = COM_RX_DONE;
    } break;

    default: {

    } break;
  }
}

/** @brief Calculates checksum byte for messages
 *
 * Currently a simple, innefective XOR placeholder
 *
 * @param _message Message to calculate checksum byte for
 *
 * @returns Calculated checksum byte
 */
uint8_t com_calc_checksum(volatile COM_Message* _message) {
  uint8_t _checksum = 0;

  _checksum ^= _message->type;
  _checksum ^= _message->length;

  uint8_t i;
  for (i = 0; i < _message->length; i++) {
    _checksum ^= _message->data[i];
  }

  return _checksum;
}


/*** ISRS ***/

ISR(USART_RX_vect) {
  // If we were receiving a message, keep going
  if (com_status & (1 << COM_RX_BUSY)) {
    com_rec_state_machine();
  // Otherwise, check and see if there's a new message
  } else {
    uint8_t _char_buffer = COM_RECV();

    if (_char_buffer == SOM) {
      // Reset state machine and start
      com_rx_buffer.state = COM_RX_INIT;
      com_rec_state_machine();
    }
  }
}

ISR(USART_TX_vect) {
  // If we started sending a message, keep going
  if (com_status & (1 << COM_TX_BUSY)) {
    com_send_state_machine();
  }
}
