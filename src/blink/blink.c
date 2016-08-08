/*
 * Minimal "blinky" test.
 *
 * Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>
 *
 * Please see the LICENSE file for the licensing terms.
 */
#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRB |= 0x2; /* Data Direction Register for Port B: 0x2 is OUT. */

    while(1) {
        PORTB ^= 0x2;
        _delay_ms(1000);
    }

    return 0;
}
