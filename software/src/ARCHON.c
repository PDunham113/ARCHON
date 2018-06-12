#include <avr/io.h>
#include <util/delay.h>

#include "COMM.h"
#include "UART.h"


/*** CONFIGURATION ***/

#define BAUD_RATE 9600UL


/*** VARIABLES ***/

COM_Message message;


/*** FUNCTION DECLARATIONS ***/


/*** BODY ***/

int main(void) {
  // Setup
  DDRB |= (1 << PINB5);
  PORTB |= (1 << PINB5);
  urt_init(BAUD_RATE);
  sei();

  // Generate test message
  message.type = COM_PKT_TEST;
  message.data[0] = 'D';
  message.data[1] = 'E';
  message.data[2] = 'A';
  message.data[3] = 'D';
  message.data[4] = 'B';
  message.data[5] = 'E';
  message.data[6] = 'E';
  message.data[7] = 'F';
  message.data[8] = '_';
  message.data[9] = 'F';
  message.data[10] = 'O';
  message.data[11] = 'O';
  message.data[12] = 'B';
  message.data[13] = 'A';
  message.data[14] = 'R';
  message.length = 15;

  while(1) {
    // Loop
    _delay_ms(1000);
    PORTB ^= (1 << PINB5);
    com_send_packet(message);
  }

  return 0;
}


/*** FUNCTION DEFINITIONS ***/
