/** @file MILLIS_TIMER.h
 *  @brief AVR millis timer on TIMER0
 *
 *  Configures TIMER0 as a millis timer. Output can be accessed through
 *  `millis()`.
 *
 *  TODO: Add support for changing SPI settings at start of transaction.
 *  TODO: Add support for multiple SS pins.
 *  TODO: Add support for multiple SPI ports.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */


#ifndef MILLIS_TIMER_H_
#define MILLIS_TIMER_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>


/* -- CONFIGURATION -- */

#ifndef F_CPU
  #define F_CPU 16000000UL
#endif /* F_CPU */

#define TMR_MILLIS_PRESCALER 64

/* -- VARIABLES & DEFINITIONS -- */

static volatile uint32_t _millis;


/* -- PUBLIC FUNCTIONS -- */

/** @brief Retrieve number of milliseconds since start.
 *
 *  Wrapper for atomic access to _millis.
 *
 *  NOTE: Will overflow approx. once every 40 days.
 *
 *  @returns milliseconds since start
 */
 uint32_t millis(void);

/** @brief Initialize TIMER0 as a 1-millisecond timer.
 *
 *  Number of milliseconds since start are tracked in `_millis`, an unsigned
 *  32-bit integer. Use `millis()` to access value, as it ensures atomic
 *  access and will cast away the `volatile` tag.
 *
 *  Timer is placed in CTC mode and configured to trigger the Compare Match A
 *  interrupt every millisecond.
 *
 *  NOTE: Any previous configuration of TIMER0 will be overwritten.
 *  WARNING: OCR0A and `timer_millis_start()` will need to change if F_CPU is
 *           not 16 MHz.
 */
 void tmr_millis_init(void);

/** @brief Start millisecond timer (TIMER0).
 *
 *  Currently configured for a /64 prescaler
 *
 *  WARNING: `timer_millis_start()` will need to change if F_CPU is
 *           not 16 MHz.
 */
 void tmr_millis_start(void);

 /// @brief Stop millisecond timer (TIMER0).
 void tmr_millis_stop(void);


#endif /* MILLIS_TIMER_H_ */
