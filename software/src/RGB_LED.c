/** @file RGB_LED.c
 *  @brief WS2812B LED Controller
 *
 *  This contains the implementation for the interface described in
 *  `RGB_LED.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 */

#include "RGB_LED.h"


/* -- VARIABLES -- */

rgb_t rgb_led[RGB_NUM_LEDS];


/* -- PUBLIC FUNCTIONS -- */

void rgb_clear(void) {
  for (uint16_t _led = 0; _led < RGB_NUM_LEDS; _led++) {
    rgb_led[_led].red = 0;
    rgb_led[_led].green = 0;
    rgb_led[_led].blue = 0;
  }
}

void rgb_init(void) {
  // Initialize SPI for sending signals
  spi_settings_t _rgb_settings = {
    .bus_mode = SPI_MASTER,
    .bit_order = SPI_MSB_FIRST,
    .interrupt = SPI_NO_INT,
    .clk_mode = SPI_MODE_0,
    .speed = SPI_DIV_4,
  };
  spi_init(_rgb_settings);
  SPI_DDR &= ~(1 << SPI_MOSI);
}

void rgb_push(void) {
  // Kill the interrupts
  uint8_t sreg = SREG;
  cli();
  // Write bits - Green - Red - Blue
  for (uint16_t led_pos = 0; led_pos < RGB_NUM_LEDS; led_pos++) {
    for (uint8_t bit_pos = RGB_MAX_BIT_POS; bit_pos != 0; bit_pos >>= 1) {
      rgb_write_bit(rgb_led[led_pos].green & bit_pos);
    }
    for (uint8_t bit_pos = RGB_MAX_BIT_POS; bit_pos != 0; bit_pos >>= 1) {
      rgb_write_bit(rgb_led[led_pos].red & bit_pos);
    }
    for (uint8_t bit_pos = RGB_MAX_BIT_POS; bit_pos != 0; bit_pos >>= 1) {
      rgb_write_bit(rgb_led[led_pos].blue & bit_pos);
    }
  }
  // Restart the interrupts, if needed
  SREG = sreg;
}
