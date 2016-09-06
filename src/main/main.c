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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <cumae/base.h>
#include <cumae/display.h>

#include <frames/test_vlines.h>

#include "def_frames.h"

static uint8_t line_buffer[57];

/*
 * SW_PAGE_INT handler.
 */
ISR(INT0_vect, ISR_BLOCK)
{
    printf("SW_PAGE_INT!\r\n");
}

int main()
{
    cumae_usart_init();

    printf("\r\n*** Cumae %s.\r\n", CUMAE_VERSION);
    printf("*** Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>\r\n\r\n");

    /* STATUS_LED (PB1) as output, ON for OK. */
    DDRB |= 1 << PB1;
    PORTB |= 1 << PB1;

    /* Enable interrupts on INT0 on rising signal. */
    EIMSK = 0x1;
    EICRA = 0x3;
    sei();

    /* Display power up sequence. */
    cumae_display_power_up();
    cumae_display_send_data(0x0A, all_white, sizeof(all_white));
    cumae_display_send_command(0x02, 0x07);
    cumae_delay_ms(1000);
    cumae_display_send_data(0x0A, all_black, sizeof(all_black));
    cumae_display_send_command(0x02, 0x07);
    cumae_delay_ms(1000);
    cumae_display_send_data(0x0A, all_white, sizeof(all_white));
    cumae_display_send_command(0x02, 0x07);
    cumae_delay_ms(1000);

    uint8_t line_counter;
    uint8_t line_buffer_len = sizeof(line_buffer);
    uint8_t sync_offset, sync_index, bc;

    for(line_counter = 0; line_counter < 96; ++line_counter) {

        /* Prepare line_buffer. */
        memset(line_buffer, 0, line_buffer_len);
        for (bc = 0; bc < 16; ++bc)
            line_buffer[bc] = pgm_read_byte(&test_vlines_frame_data[(line_counter * 32) + bc]);

        /* Sync byte. */
        sync_offset = 23 - (line_counter  / 4);
        sync_index = line_counter % 4;
        line_buffer[16 + sync_offset] = 0x3 << (sync_index * 2);

        for (bc = 0; bc < 16; ++bc)
            line_buffer[40 + bc] = pgm_read_byte(&test_vlines_frame_data[(line_counter * 32) + 16 + bc]);

        /* Send line_buffer. */
        cumae_display_send_data(0x0A, line_buffer, line_buffer_len);
        cumae_display_send_command(0x02, 0x07);
    }

    cumae_delay_ms(500);
    cumae_display_power_off();

    /* Idle loop - Don't put anything here.:) */
    while(1) {
    }

    return 0;
}
