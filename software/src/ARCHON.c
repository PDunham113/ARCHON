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
#define LED_UPDATE_PERIOD 500 // [ms]


/*** VARIABLES ***/

/*const com_message_t messages[] = {
  {.type = COM_PKT_TEST, .length = 5, .data = "INIT\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = "START\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = " LOOP\n"},
  {.type = COM_PKT_TEST, .length = 6, .data = " SENT\n"},
};*/


/*** FUNCTION DECLARATIONS ***/

void init_all(void);


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
