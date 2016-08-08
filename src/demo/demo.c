/*
 * Minimal "blinky" demo.
 */
#include <avr/io.h>
#include <util/delay.h>

int main() {
    DDRB |= 0x2; /* Data Direction Register for Port B: 0x2 is OUT. */

    while(1) {
        PORTB ^= 0x2;
        _delay_ms(5000);
    }

    return 0;
}
