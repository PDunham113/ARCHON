/** @file ARCHON.c
 *  @brief Addressable RGB LED Controller.
 *
 *  Still **far** from being complete.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "COMM.h"
#include "MILLIS_TIMER.h"
#include "RGB_LED.h"
#include "UART.h"


/*** CONFIGURATION ***/

#define BAUD_RATE 9600UL // [baud]


/*** VARIABLES ***/

const com_message_t BUSY_MSG = {.type = COM_PKT_BUSY};
const com_message_t READY_MSG = {.type = COM_PKT_READY};

uint8_t g_msg_ok_to_send = 0;
uint16_t g_led_block = 0;

/*** FUNCTION DECLARATIONS ***/

void init_all(void);
void process_incoming_message(void);
void push_to_led(void);

/*** BODY ***/

int main(void) {
  // Setup all devices
  init_all();

  sei();

  while(1) {
    // Check for finished incoming messages.
    if (com_status & (1 << COM_RX_READY)) {
      process_incoming_message();
    }
  }

  return 0;
}


/*** FUNCTION DEFINITIONS ***/

/** @brief Initialize all modules
 *
 * All initialization functions are grouped here. Interrupts are *not* enabled
 * in this function.
 */
void init_all(void) {
  rgb_init();
  tmr_millis_init();
  urt_init(BAUD_RATE);
}

/** @brief Process the incoming COMM message
 *
 *  The following types of messages are supported:
 *    COM_PKT_BUSY
 *      Sets `g_msg_ok_to_send` to 0, blocking sent messages.
 *    COM_PKT_LED_CTRL
 *      CHANGE_BLOCK
 *        Update active LED block
 *      COPY
 *        Copy one LED to others
 *      ERASE
 *        Clear all LEDs
 *      PUSH
 *        Update LEDs
 *    COM_PKT_LED_DATA
 *      Write each LED data segment to the correct LED within the block.
 *    COM_PKT_READY
 *      Sets `g_msg_ok_to_send` to 1, allowing sent messages.
 *  @returns Void.
 */
void process_incoming_message(void) {
  // Grab received packet
  com_message_t _rec_pkt = com_rec_packet();

  // Process packet.
  switch (_rec_pkt.type) {
    // Ensure we don't send any more packets
    case COM_PKT_BUSY: {
      g_msg_ok_to_send = 0;
    } break;
    // Process LED control commands
    case COM_PKT_LED_CTRL: {
      // TODO: Support ALL the commands
      switch (_rec_pkt.data[0]) {
        // CHANGE_BLOCK ('B')
        case 0x42: {
          g_led_block = _rec_pkt.data[1];
        } break;
        // COPY ('C')
        case 0x43: {
          uint16_t _cpy_idx = g_led_block & _rec_pkt.data[1];
          for (uint8_t _led = 2; _led < _rec_pkt.length; _led++) {
            rgb_led[g_led_block & _rec_pkt.data[_led]] = rgb_led[_cpy_idx];
          }
        } break;
        // ERASE ('E')
        case 0x45: {
          rgb_clear();
        } break;
        // PUSH ('P')
        case 0x50: {
          push_to_led();
        } break;
      }
    } break;
    // Set RGB_LED buffer to new data
    case COM_PKT_LED_DATA: {
      for(uint8_t _led = 0; _led < _rec_pkt.length; _led += 4) {
        uint16_t _rgb_idx = g_led_block & _rec_pkt.data[_led];
        rgb_led[_rgb_idx].red = _rec_pkt.data[_led + 1];
        rgb_led[_rgb_idx].green = _rec_pkt.data[_led + 2];
        rgb_led[_rgb_idx].blue = _rec_pkt.data[_led + 3];
      }
    } break;
    // Allow us to send more packets
    case COM_PKT_READY: {
      g_msg_ok_to_send = 1;
    } break;
  }
}

/** @brief Push new data to LED strip, inform the rest of the world
 *
 *  Send a COM_PKT_BUSY, push to LEDs & latch, then send a COM_PKT_READY.
 *
 *  @returns Void.
 */
void push_to_led(void) {
  com_send_packet(BUSY_MSG);
  rgb_push();
  _delay_us(5);
  com_send_packet(READY_MSG);
}
