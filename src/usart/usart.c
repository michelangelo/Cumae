/*
 * USART test.
 */
#define F_CPU 8000000
#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
//#include <avr/interrupt.h>

void usart_init() {
    UBRR0H = (uint8_t)UBRRH_VALUE;
    UBRR0L = (uint8_t)UBRRL_VALUE;
#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(TXEN0);   /* Enable RX and TX */

#if 0
    UCSR0B = 1 << TXEN0; 
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);
#endif
}

void usart_tx(const uint8_t data) {
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;
#if 0
    while(!(UCSR0A & (1<<UDRE0)))
        ;
    UDR0 = data;
#endif
}

int main() {
    usart_init();
    DDRB |= 0x2; /* Data Direction Register for Port B: 0x2 is OUT. */

    while(1) {
        PORTB ^= 0x2;
        usart_tx('O');
        usart_tx('K');
        usart_tx('\r');
        usart_tx('\n');
        _delay_ms(1000);
    }

    return 0;
}
