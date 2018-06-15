#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>

#include "COMM.h"
#include "UART.h"


/*** CONFIGURATION ***/

#define BAUD_RATE 9600UL // [baud]
#define LED_UPDATE_PERIOD 500 // [ms]


/*** VARIABLES ***/

// Private
static volatile uint32_t _millis;

// Public
COM_Message message = {.type = COM_PKT_TEST, .length = 5, .data = "INIT\n"};
uint32_t last_led_update;

/*** FUNCTION DECLARATIONS ***/

void init_all(void);

uint32_t millis(void);

void tmr_millis_init(void);
void tmr_millis_start(void);
void tmr_millis_stop(void);


/*** BODY ***/

int main(void) {
  init_all();
  sei();

  // Start 1ms timer
  tmr_millis_start();

  com_send_packet(message);

  // Configure pins
  DDRB |= (1 << PINB5);
  PORTB |= (1 << PINB5);

  while(1) {
    uint32_t _now = millis();

    if (_now - last_led_update >= LED_UPDATE_PERIOD) {
      last_led_update = _now;
      PORTB ^= (1 << PINB5);
      com_send_packet(message);
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
  tmr_millis_init();
  urt_init(BAUD_RATE);
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

/// @brief TIMER0 millis timer interrupt
ISR(TIMER0_COMPA_vect) {
  _millis++;
}
