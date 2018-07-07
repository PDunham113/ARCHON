/** @file RGB_LED.h
 *  @brief WS2812B LED Controller
 *
 *  Uses SPI to generate bit pulses. Interrupt-driven so *some* work may be
 *  possible between bits.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#ifndef RGB_LED_H
#define RGB_LED_H

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay_basic.h>

#include "SPI.h"


/* -- CONFIGURATION -- */

#define RGB_NUM_LEDS 20


/* -- VARIABLES & DEFINITIONS -- */

#define RGB_MAX_BIT_POS 0b10000000
#define RGB_HIGH_BIT    0xE0
#define RGB_LOW_BIT     0x80

// TODO: Find a better way to address this...
typedef struct rgb_ {
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} rgb_t;

extern rgb_t rgb_led[RGB_NUM_LEDS];


/* -- PUBLIC FUNCITONS -- */

/** @brief Clear RGB LED Array
 *
 *  Sets each element of the RGB array to 0. Does not push elements to array.
 */
void rgb_clear(void);

/** @brief Initialize RGB LED controller
 *
 *  Initializes SPI in master mode with /4 prescaler, mode 0, MSB first, and no
 *  interrupts. SPI MOSI is configured as input ASAP. This must be combined
 *  with a pulldown resistor.
 *
 *  @returns Void.
 */
void rgb_init(void);

/** @brief Push updates to LED strip.
 *
 *  The rgb_led array is pushed to the LEDs in GRB order as fast as possible
 *  using SPI. A delay of at least 6us must occur between calls for the LEDs
 *  to latch. Interrupts will be disabled for the entirety of the function.
 *
 * NOTE: Configured for 16 MHz clock.
 *
 *  @returns Void.
 */
void rgb_push(void);


/* -- PRIVATE FUNCTIONS -- */

/** @brief Write one byte to RGB LED
 *
 *  Wrapper for SPI write that ensures MOSI is low between bits.
 *
 *  NOTE: Configured for 16 MHz clock.
 *
 *  @param _bit The bit to send to the WS2812B. Anything not 0 is 1.
 *  @returns Void.
 */
static inline void rgb_write_bit(uint8_t _bit) {
  uint8_t _write = (_bit) ? RGB_HIGH_BIT : RGB_LOW_BIT;
  SPI_DDR |= (1 << SPI_MOSI);
  spi_send(_write);
  _delay_loop_1(3);
  SPI_DDR &= ~(1 << SPI_MOSI);
}

#endif /* RGB_LED_H */
