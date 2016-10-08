/*
 * MIT License
 *
 * Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <cumae/base.h>

/*
 * Default baud rate for USART.
 */
#ifndef BAUD
#define BAUD 9600
#endif

#include <stdio.h>
#include <avr/io.h>
#include <string.h>
#include <util/setbaud.h>

const char *cm_ver_git_hash = _CM_VER_GIT_HASH;
const char *cm_ver_build_date = _CM_VER_BUILD_DATE;

/*
 * TODO:
 *  Internal init context to keep track of states.
 */

static int _cm_usart_putchar(char c, FILE *stream);
static FILE cm_stdout = FDEV_SETUP_STREAM(_cm_usart_putchar,
                                             NULL, _FDEV_SETUP_WRITE);

#if 0
static struct cm_context_s _cm_ctx;

cm_err_t cm_init(struct cm_context_s *ctx)
{
    memset(&_cm_ctx, 0, sizeof(struct cm_context_s));
    memcpy(&_cm_ctx, ctx, sizeof(struct cm_context_s));

    return ENONE;
}
#endif

void cm_usart_init(void)
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
    stdout = &cm_stdout;
}

inline cm_byte_t cm_usart_tx(const cm_byte_t data)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = data;

    return UDR0;
}

/*
 * *** SPI FUNCTIONS ***
 */
void cm_spi_init(void)
{
    /* Set MOSI and SCK output, all others input. */
    DDRB |= (1 << PB3) | (1 << PB5);

    /*
     * Enable SPI as Master.
     * Clock prescaler is defaulted to F_CPU/4.
     */
    SPCR = (1<<SPE) | (1<<MSTR);
}

inline cm_byte_t cm_spi_w1r1(cm_byte_t data)
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

inline void cm_print(cm_print_level_t level, char *s)
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
 * TODO: This really needs to be reviewed as it might
 * drift and/or be very imprecise.
 */
inline void cm_delay_ms(uint16_t ms)
{
    for(; 0 < ms; --ms)
        _delay_ms(1);
}

/*
 * Internal function.
 */
static int _cm_usart_putchar(char c, FILE *stream)
{
    cm_usart_tx(c);
    return 0;
}

