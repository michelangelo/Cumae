#include <cumae/base.h>

/*
 * Default baud rate for USART.
 */
#ifndef BAUD
#define BAUD 9600
#endif

#include <stdio.h>
#include <avr/io.h>
#include <util/setbaud.h>

/*
 * TODO:
 *  Internal init context to keep track of states.
 */

static int _cumae_usart_putchar(char c, FILE *stream);
static FILE cumae_stdout = FDEV_SETUP_STREAM(_cumae_usart_putchar,
                                             NULL, _FDEV_SETUP_WRITE);

void cumae_usart_init(void)
{
    /* UBRR_VALUE is globally defined in avr-libc. */
    UBRR0 = (cm_byte_t)UBRR_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    /* 8 bit data. */
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);

    /* TX only. */
    UCSR0B = _BV(TXEN0);

    /* Moving stdout on the USART. */
    stdout = &cumae_stdout;
}

inline cm_byte_t cumae_usart_tx(const cm_byte_t data)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;

    return UDR0;
}

/*
 * *** SPI FUNCTIONS ***
 */
void cumae_spi_init(void)
{
    /* Set MOSI and SCK output, all others input. */
    DDRB |= (1 << PB3) | (1 << PB5);

    /*
     * Enable SPI as Master.
     * Clock prescaler is defaulted to F_CPU/4.
     */
    SPCR = (1<<SPE) | (1<<MSTR);
}

inline cm_byte_t cumae_spi_w1r1(cm_byte_t data)
{
    /* Start transmission */
    SPDR = data;

    /* Wait for transmission complete */
    loop_until_bit_is_set(SPSR, SPIF);
#if 0
    while(!(SPSR & (1<<SPIF)))
        ;
#endif

    return SPDR;
}

inline void cumae_print(cumae_print_level_t level, char *s)
{
    if (level == NONE)
        return;

    switch (level) {
        case ERROR:
            printf("ERROR: %s\r\n", s);
            break;
        case DEBUG:
            printf("D: %s\r\n", s);
            break;
        case NORMAL:
        default:
            printf("%s\r\n", s);
    }
}

/*
 * Internal function.
 */
static int _cumae_usart_putchar(char c, FILE *stream)
{
    cumae_usart_tx(c);
    return 0;
}

