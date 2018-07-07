/** @file SPI.h
 *  @brief Driver for ATMEGA328P SPI
 *
 *  Minimal SPI driver for the above. Not compatible with `fprintf()`. Places
 *  device in master mode.
 *
 *  TODO: Add support for changing SPI settings at start of transaction.
 *  TODO: Add support for multiple SS pins.
 *  TODO: Add support for multiple SPI ports.
 *
 *  @author Patrick Dunham
 *  @bug No known bugs.
 *  @version 0.0.1
 */

#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <stdint.h>


/* -- VARIABLES & DEFINITIONS -- */

// SPI Port Definitions
#define SPI_DDR  DDRB
#define SPI_PORT PORTB
#define SPI_PIN  PINB

// SPI Pin Definitions
#define SPI_MOSI PINB3
#define SPI_MISO PINB4
#define SPI_SCK  PINB5
#define SPI_SS   PINB2

// SPI Bit Order
#define SPI_MSB_FIRST 0
#define SPI_LSB_FIRST 1

// SPI Bus Mode
#define SPI_MASTER    0
#define SPI_SLAVE     1

// SPI Clock Mode
#define SPI_MODE_0    0
#define SPI_MODE_1    1
#define SPI_MODE_2    2
#define SPI_MODE_3    3

// SPI Clock Dividers
#define SPI_DIV_2     0
#define SPI_DIV_4     1
#define SPI_DIV_8     2
#define SPI_DIV_16    3
#define SPI_DIV_32    4
#define SPI_DIV_64    5
#define SPI_DIV_64_2X 6 // /128 with SPI2X enabled - should equal SPI_DIV_64
#define SPI_DIV_128   7

// SPI Interrupts
#define SPI_INT       0
#define SPI_NO_INT    1

typedef struct spi_settings_ {
  uint8_t bus_mode  : 1;
  uint8_t bit_order : 1;
  uint8_t interrupt : 1;
  uint8_t clk_mode  : 2;
  uint8_t speed     : 3;
} spi_settings_t;

// Private
static spi_settings_t spi_default_settings;

/* -- PUBLIC FUNCTIONS -- */

/** @brief Start SPI transmission
 *
 *  Pulls SS pin low using default SPI settings. Call to start sending a series
 *  of bytes over SPI. Currently uses the chip default SS pin.
 */
 inline void spi_begin_transmission(void) {
   SPI_PORT &= ~(1 << SPI_SS);
 }

/** @brief End SPI transmission
 *
 *  Pulls SS pin high using default SPI settings. Call to end sending a series
 *  of bytes over SPI. Currently uses the chip default SS pin.
 */
 inline void spi_end_transmission(void) {
   SPI_PORT |= (1 << SPI_SS);
 }

/** @brief Initialize SPI bus to specified parameters
 *
 *  These settings will be used if none are specified in
 *  `spi_begin_transmission`. Master mode will set MOSI, SCK, and SS as
 *  outputs, and MISO as an input, as well as pull SS high. Slave mode will set
 *  MOSI, SCK, SS as inputs, and MISO as an output. It is assumed that SS is
 *  the default specified by the device - only one SS is currently supported.
 *
 *  NOTE: Any previous SPI configuration will be overwritten.
 *
 *  @param _default_settings The settings to initialize the SPI bus to.
 *  @returns Void.
 */
void spi_init(spi_settings_t _default_settings);

/** @brief Recieves a character from the SPI buffer.
 *
 *  Will return immediatly with the buffer contents.
 *
 *  @returns The raw byte received over SPI
 */
inline uint8_t spi_recv(void) {
  return SPDR;
}

/** @brief Recieves a character from the SPI buffer, blocking for valid data.
 *
 *  Will return when a character has been received over SPI
 *
 *  @returns The raw byte received over SPI
 */
uint8_t spi_recv_block(void);

/** @brief Recieves a character from the SPI buffer, blocking w/ timeout.
 *
 *  Will return when a character has been received over SPI, or when the
 *  timeout expires. The timeout is expressed as the number of loop iterations
 *  in the function below. Experimentation is needed to determine a real-world
 *  time equivalent.
 *
 *  @param _timeout Max time to wait for new character, in arbitrary units
 *  @returns The raw byte received over SPI
 */
uint8_t spi_recv_timeout(uint32_t _timeout);

/** @brief Queues a single character to be sent over SPI.
 *
 *  Will return immediatly.
 *
 *  @param _data The raw byte to send over SPI
 *  @returns Void.
 */
 inline void spi_send(uint8_t _data) {
   SPDR = _data;
 }

/** @brief Sends a character over SPI, blocking until it is sent.
 *
 *  Will return when the character is sent
 *
 *  @param _data The raw byte to send over SPI
 *  @returns Void.
 */
void spi_send_block(uint8_t _data);


#endif /* SPI_H */
