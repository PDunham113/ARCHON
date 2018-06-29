/** @file MILLIS_TIMER.c
 *  @brief AVR millis timer on TIMER0
 *
 *  This contains the implementation for the interface described in
 *  `MILLIS_TIMER.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 */

#include "MILLIS_TIMER.h"

/* -- VARIABLES -- */

static volatile uint32_t _millis = 0;


/* -- PUBLIC FUNCTIONS -- */

uint32_t millis(void) {
  uint32_t _millis_buffer;

  // Store current interrupt status before disabling.
  uint8_t _sreg = SREG;
  cli();
  _millis_buffer = (uint32_t)_millis;
  SREG = _sreg;

  return _millis_buffer;
}


void tmr_millis_init(void) {
  TCCR0A = (1 << WGM01);
  TCCR0B = 0;
  TIMSK0 = (1 << OCIE0A);
  OCR0A = (F_CPU / 1000 / TMR_MILLIS_PRESCALER) - 1;
}


void tmr_millis_start(void) {
  TCCR0B = (1 << CS01) | (1 << CS00);
}


void tmr_millis_stop(void) {
  TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}


/* -- ISRS -- */

ISR(TIMER0_COMPA_vect) {
  _millis++;
}
