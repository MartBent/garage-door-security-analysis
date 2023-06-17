#define __AVR_ATtiny85__
#define F_CPU 1000000UL

#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>

static volatile bool do_send = false;

void high() { PORTB |= (1 << PB4); }
void low() { PORTB &= ~(1 << PB4); }

void start() {
  for (int i = 0; i < 12; i++) {
    high();
    _delay_us(400);
    low();
    _delay_us(400);
  }
}

void symbol(bool val) {
  high();
  _delay_us(400);
  if (val) {
    high();
  } else {
    low();
  }
  _delay_us(400);
  low();
  _delay_us(400);
}

void code(uint8_t c) {
  int data[8] = {0xDF, 0xDB, c, c, 0x54, 0xE4, 0xA1, 0xDB};
  for (int i = 0; i < 8; i++) {
    for (int x = 7; x >= 0; x--) {
      int val = (data[i] >> x) & 1;
      symbol((bool)val);
    }
  }
}

void trigger_door() {
  static uint8_t rol_code = 0;
  // Start pulses
  start();
  _delay_us(4000); // 4 MS
  // Actual "Rolling" code
  code(rol_code++);

  // Ending signal
  symbol(false);
  symbol(false);

  _delay_us(16000); // 16 MS
}

ISR(INT0_vect) { do_send = true; }

int main() {

  // Enable the data pin of the transmitter
  DDRB |= 1 << PB4;

  // Enable interrupts
  PORTB |= 1 << PB2;
  sei();
  GIMSK |= 1 << INT0;
  MCUCR |= 2;

  while (true) {
    if (do_send) {
      trigger_door();
      _delay_us(500000);
      do_send = false;
    }
  }
}
