/** @file SPI.c
 *  @brief Driver for ATMEGA328P SPI
 *
 *  This contains the implementation for the interface described in `SPI.h`.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#include "SPI.h"


/* -- PUBLIC FUNCTIONS -- */

void spi_init(spi_settings_t _default_settings) {
  // Set SPI pins to proper direction
  if (_default_settings.bus_mode == SPI_MASTER) {
    SPI_DDR |= (1 << SPI_MOSI) | (1 << SPI_SS) | (1 << SPI_SCK);
    SPI_DDR &= ~(1 << SPI_MISO);

    // Pull SS high (no transmission)
    SPI_PORT |= (1 << SPI_SS);

    SPCR = (1 << SPE) | (1 << MSTR);
  } else {
    SPI_DDR &= ~(1 << SPI_MOSI) | (1 << SPI_SS) | (1 << SPI_SCK);
    SPI_DDR |= (1 << SPI_MISO);

    SPCR = (1 << SPE);
  }

  // Enable interrupts (if specified)
  if (_default_settings.interrupt == SPI_INT) {
    SPCR |= (1 << SPIE);
  }

  // Flip bit order (if specified)
  if (_default_settings.bit_order == SPI_LSB_FIRST) {
    SPCR |= (1 << DORD);
  }

  // Choose SPI clock mode
  switch (_default_settings.clk_mode) {
    case SPI_MODE_0: {
      // No changes needed
    } break;
    case SPI_MODE_1: {
      SPCR | (1 << CPHA);
    } break;
    case SPI_MODE_2: {
      SPCR | (1 << CPOL);
    } break;
    case SPI_MODE_3: {
      SPCR | (1 << CPOL) | (1 << CPHA);
    } break;
  }

  // Choose SPI clock divider
  switch (_default_settings.speed) {
    case SPI_DIV_2: {
      SPSR = (1 << SPI2X);
    } break;
    case SPI_DIV_4: {
      SPSR = 0;
    } break;
    case SPI_DIV_8: {
      SPCR |= (1 << SPR0);
      SPSR = (1 <<SPI2X);
    } break;
    case SPI_DIV_16: {
      SPCR |= (1 << SPR0);
      SPSR = 0;
    } break;
    case SPI_DIV_32: {
      SPCR |= (1 << SPR1);
      SPSR = (1 << SPI2X);
    } break;
    case SPI_DIV_64: {
      SPCR |= (1 << SPR1);
      SPSR = 0;
    } break;
    case SPI_DIV_64_2X: {
      SPCR |= (1 << SPR1) | (1 << SPR0);
      SPSR = (1 << SPI2X);
    } break;
    case SPI_DIV_128: {
      SPCR |= (1 << SPR1) | (1 << SPR0);
      SPSR = 0;
    } break;
  }

  spi_default_settings = _default_settings;
}

uint8_t spi_recv_block(void) {
  // Wait to receive
  while(!(SPSR & (1 << SPIF))) { }

  return SPDR;
}

uint8_t spi_recv_timeout(uint32_t _timeout) {
  // Wait to receive
  do {
    // Check for reception
    if((SPSR & (1 << SPIF))) {
      _timeout = 0;
    }
  } while(_timeout--);

  return SPDR;
}

void spi_send_block(uint8_t _data) {
  SPDR = _data;

  // Wait to send
  while(!(SPSR & (1 << SPIF))) { }
}
