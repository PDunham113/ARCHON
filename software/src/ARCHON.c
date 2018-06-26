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

//#include "COMM.h"
#include "RGB_LED.h"
//#include "UART.h"


/*** CONFIGURATION ***/

#define BAUD_RATE 9600UL // [baud]
#define LED_UPDATE_PERIOD 500 // [ms]


/*** VARIABLES ***/

// Private
static volatile uint32_t _millis;

// Public
/*const com_message_t messages[] = {
  {.type = COM_PKT_TEST, .length = 5, .data = "INIT\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = "START\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = " LOOP\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = " SENT\n"},
};*/


/*** FUNCTION DECLARATIONS ***/

void init_all(void);

uint32_t millis(void);

void tmr_millis_init(void);
void tmr_millis_start(void);
void tmr_millis_stop(void);

//void print_uint32_hex(uint32_t _num);


/*** BODY ***/

int main(void) {
  rgb_led[0].red   = 0xFF;
  rgb_led[0].green = 0x00;
  rgb_led[0].blue  = 0x00;

  rgb_led[1].red   = 0xFF;
  rgb_led[1].green = 0x7F;
  rgb_led[1].blue  = 0x00;

  rgb_led[2].red   = 0xFF;
  rgb_led[2].green = 0xFF;
  rgb_led[2].blue  = 0x00;

  rgb_led[3].red   = 0x00;
  rgb_led[3].green = 0xFF;
  rgb_led[3].blue  = 0x00;

  rgb_led[4].red   = 0x00;
  rgb_led[4].green = 0x00;
  rgb_led[4].blue  = 0xFF;

  rgb_led[5].red   = 0x4B;
  rgb_led[5].green = 0x00;
  rgb_led[5].blue  = 0x82;

  rgb_led[6].red   = 0x94;
  rgb_led[6].green = 0x00;
  rgb_led[6].blue  = 0xD3;

  rgb_led[7].red   = 0xFF;
  rgb_led[7].green = 0x00;
  rgb_led[7].blue  = 0x00;

  rgb_led[8].red   = 0x00;
  rgb_led[8].green = 0xFF;
  rgb_led[8].blue  = 0x00;

  rgb_led[9].red   = 0x00;
  rgb_led[9].green = 0x00;
  rgb_led[9].blue  = 0xFF;

  rgb_led[10].red   = 0xFF;
  rgb_led[10].green = 0xFF;
  rgb_led[10].blue  = 0xFF;

  rgb_led[11].red   = 0x00;
  rgb_led[11].green = 0x00;
  rgb_led[11].blue  = 0xFF;

  rgb_led[12].red   = 0xFF;
  rgb_led[12].green = 0x00;
  rgb_led[12].blue  = 0x00;

  rgb_led[13].red   = 0x00;
  rgb_led[13].green = 0xFF;
  rgb_led[13].blue  = 0x00;

  rgb_led[14].red   = 0x00;
  rgb_led[14].green = 0x00;
  rgb_led[14].blue  = 0xFF;

  rgb_led[15].red   = 0xFF;
  rgb_led[15].green = 0x00;
  rgb_led[15].blue  = 0x00;

  rgb_led[16].red   = 0x00;
  rgb_led[16].green = 0xFF;
  rgb_led[16].blue  = 0x00;

  rgb_led[17].red   = 0x00;
  rgb_led[17].green = 0x00;
  rgb_led[17].blue  = 0xFF;

  rgb_led[18].red   = 0x00;
  rgb_led[18].green = 0xFF;
  rgb_led[18].blue  = 0x00;

  rgb_led[19].red   = 0x00;
  rgb_led[19].green = 0x00;
  rgb_led[19].blue  = 0xFF;

  rgb_init();
  _delay_us(50);
  rgb_push();

  while(1) {

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
  tmr_millis_init();
}

/** @brief Retrieve number of milliseconds since start
 *
 * Wrapper for atomic access to _millis.
 *
 * NOTE: Will overflow approx. once every 40 days.
 *
 * @returns milliseconds since start
 */
uint32_t millis(void) {
  uint32_t _millis_buffer;

  // Store current interrupt status before disabling.
  uint8_t _sreg = SREG;
  cli();
  _millis_buffer = (uint32_t)_millis;
  SREG = _sreg;

  return _millis_buffer;
}

/** @brief Initialize TIMER0 as a 1-millisecond timer
 *
 * Number of milliseconds since start are tracked in `_millis`, an unsigned
 * 32-bit integer. Use `millis()` to access value, as it ensures atomic access
 * and will cast away the `volatile` tag.
 *
 * Timer is placed in CTC mode and configured to trigger the Compare Match A
 * interrupt every millisecond.
 *
 * NOTE: Any previous configuration of TIMER0 will be overwritten.
 * WARNING: OCR0A and `timer_millis_start()` will need to change if F_CPU is
 *          not 16 MHz.
 */
void tmr_millis_init(void) {
  TCCR0A = (1 << WGM01);
  TCCR0B = 0;
  TIMSK0 = (1 << OCIE0A);
  OCR0A = 249;
}

/** @brief Start millisecond timer (TIMER0)
 *
 * Currently configured for a /64 prescaler
 *
 * WARNING: OCR0A and `timer_millis_start()` will need to change if F_CPU is
 *          not 16 MHz.
 */
void tmr_millis_start(void) {
  TCCR0B = (1 << CS01) | (1 << CS00);
}

/// @brief Stop millisecond timer (TIMER0)
void tmr_millis_stop(void) {
  TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}

/// @brief Print 32-bit unsigned int to UART in hex
/// TODO: WRITE DOCS
/*void print_uint32_hex(uint32_t _num) {
  com_message_t _hex_msg = {.type = COM_PKT_TEST, .length = 8,
                            .data = "        "};
  for (uint8_t idx = 0; idx < 8; idx++) {
    // Convert each nibble to hex
    uint8_t _nibble = (_num >> (4 * (7 - idx))) & 0b1111;
    switch (_nibble) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 6:
      case 7:
      case 8:
      case 9: {
        _hex_msg.data[idx] = '0' + _nibble;
      } break;
      case 10:
      case 11:
      case 12:
      case 13:
      case 14:
      case 15: {
        _hex_msg.data[idx] = 'A' + _nibble - 10;
      } break;
      default:
      {
        _hex_msg.data[idx] = '#';
      } break;
    }
  }

  while (com_status & (1 << COM_TX_BUSY));
  com_send_packet(_hex_msg);
}
*/
