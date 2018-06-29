/** @file COMM.c
 *  @brief Simple packet-based communications layer.
 *
 *  This contains the implementation for the interface described in `COMM.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#include "COMM.h"

/* -- VARIABLES -- */

volatile uint8_t com_status = (1 << COM_TX_READY);
volatile com_error_t com_rx_error = COM_NO_ERROR;
volatile com_error_t com_tx_error = COM_NO_ERROR;

/* -- PUBLIC FUNCTIONS -- */

void com_send_packet(com_message_t _message) {
  if (com_status & (1 << COM_TX_READY)) {
    com_tx_buffer = _message;

    com_tx_buffer.state = COM_INIT;

    com_status &= ~(1 << COM_TX_READY);

    com_send_state_machine();
  }
}

com_message_t com_rec_packet(void) {
  com_status &= ~(1 << COM_RX_READY);

  return com_rx_buffer;
}


/* -- PRIVATE FUNCTIONS -- */

static void com_send_state_machine(void) {
  static uint8_t _data_loc;

  /* STATE MACHINE */

  switch (com_tx_buffer.state) {
    case COM_INIT: {
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

      com_tx_buffer.state = COM_TYPE;
    } break;

    case COM_TYPE: {
      COM_SEND(com_tx_buffer.type);

      com_tx_buffer.state = COM_LENGTH;
    } break;

    case COM_LENGTH: {
      COM_SEND(com_tx_buffer.length);

      _data_loc = 0;
      com_tx_buffer.state = COM_DATA;
    } break;

    case COM_DATA: {
      COM_SEND(com_tx_buffer.data[_data_loc]);
      _data_loc++;

      if (_data_loc >= com_tx_buffer.length
          || _data_loc >= COM_MAX_DATA_LENGTH) {
        com_tx_buffer.state = COM_EC;
      }
    } break;

    case COM_EC: {
      COM_SEND(com_tx_buffer.checksum);

      com_tx_buffer.state = COM_POST;
    } break;

    case COM_POST: {
      // Tell the world we're done
      com_status &= ~(1 << COM_TX_BUSY);
      com_status |= (1 << COM_TX_READY);

      com_tx_buffer.state = COM_DONE;
    } break;

    default: {

    } break;
  }
}

static void com_rec_state_machine(void) {
  static uint8_t _data_loc;

  switch (com_rx_buffer.state) {
    case COM_INIT: {
      // Tell the world we're busy
      com_status |= (1 << COM_RX_BUSY);

      com_rx_buffer.state = COM_TYPE;
    } break;

    case COM_TYPE: {
      com_rx_buffer.type = COM_RECV();

      com_rx_buffer.state = COM_LENGTH;
    } break;

    case COM_LENGTH: {
      com_rx_buffer.length = COM_RECV();

      _data_loc = 0;
      com_rx_buffer.state = COM_DATA;
    } break;

    case COM_DATA: {
      com_rx_buffer.data[_data_loc] = COM_RECV();
      _data_loc++;

      if (_data_loc >= com_rx_buffer.length
          || _data_loc >= COM_MAX_DATA_LENGTH) {
        com_rx_buffer.state = COM_EC;
      }
    } break;

    case COM_EC: {
      com_rx_buffer.checksum = COM_RECV();

      uint8_t _checksum = com_calc_checksum(&com_rx_buffer);

      if (_checksum != com_rx_buffer.checksum) {
        com_status |= (1 << COM_RX_ERROR);
        com_rx_error = COM_ERROR_EC;
      }

      com_status &= ~(1 << COM_RX_BUSY);
      com_status |= (1 << COM_RX_READY);

      com_rx_buffer.state = COM_DONE;
    } break;

    default: {

    } break;
  }
}

static uint8_t com_calc_checksum(volatile com_message_t *_message) {
  uint8_t _checksum = 0;

  _checksum ^= _message->type;
  _checksum ^= _message->length;

  uint8_t i;
  for (i = 0; i < _message->length; i++) {
    _checksum ^= _message->data[i];
  }

  return _checksum;
}


/* -- ISRS -- */

COM_RX_INTERRUPT() {
  // If we were receiving a message, keep going
  if (com_status & (1 << COM_RX_BUSY)) {
    com_rec_state_machine();
  // Otherwise, check and see if there's a new message
  } else {
    uint8_t _char_buffer = COM_RECV();

    if (_char_buffer == SOM) {
      // Reset state machine and start
      com_rx_buffer.state = COM_INIT;
      com_rec_state_machine();
    }
  }
}

COM_TX_INTERRUPT() {
  // If we started sending a message, keep going
  if (com_status & (1 << COM_TX_BUSY)) {
    com_send_state_machine();
  }
}
