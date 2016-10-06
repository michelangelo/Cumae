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
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/interrupt.h>

#include <cumae/base.h>
#include <cumae/display.h>

#include <frames/sbuca_logo_vertical.h>
#include <frames/firefox.h>
#include <frames/freebsd_vertical.h>
#include <frames/gnuhead_vertical.h>
#include <frames/eff_vertical.h>

#include "def_frames.h"

static uint8_t timer_test = 0;
static uint8_t frame_id = 0;

/*
 * SW_PAGE_INT handler.
 */
ISR(INT0_vect, ISR_BLOCK)
{
    printf("SW_PAGE_INT!\r\n");
}

ISR(TIMER0_OVF_vect, ISR_BLOCK)
{
    timer_test++;
    if (timer_test == 225) {

        cm_display_power_up();
        switch(frame_id) {
            case 0:
                cm_display_stage_update(sbuca_logo_vertical_frame_data, gnuhead_vertical_frame_data);
                frame_id++;
                break;
            case 1:
                cm_display_stage_update(gnuhead_vertical_frame_data, eff_vertical_frame_data);
                frame_id++;
                break;
            case 2:
                cm_display_stage_update(eff_vertical_frame_data, freebsd_vertical_frame_data);
                frame_id++;
                break;
            case 3:
                cm_display_stage_update(freebsd_vertical_frame_data, firefox_frame_data);
                frame_id++;
                break;
            case 4:
                cm_display_stage_update(firefox_frame_data, sbuca_logo_vertical_frame_data);
                frame_id = 0;
                break;
        }

        cm_display_power_off();

        timer_test = 0;
    }
}

int main()
{
    cm_usart_init();

    printf("\r\n*** Cumae %s.\r\n", cm_VERSION);
    printf("*** Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>\r\n\r\n");

    /* STATUS_LED (PB1) as output, ON for OK. */
    DDRB |= 1 << PB1;
    PORTB |= 1 << PB1;

    /* Enable interrupts on INT0 on rising signal. */
    EIMSK = 0x1;
    EICRA = 0x3;

    /* Timer0 overflow counter enable with a /1024 divider. */
    TCCR0B |= 0x5;
    TIMSK0 |= 0x1;

    /* Display power up sequence. */
    cm_display_power_up();
    cm_display_send_data(0x0A, all_white, sizeof(all_white));
    cm_display_send_command(0x02, 0x07);
    cm_delay_ms(500);
    cm_display_send_data(0x0A, all_black, sizeof(all_black));
    cm_display_send_command(0x02, 0x07);
    cm_delay_ms(500);
    cm_display_send_data(0x0A, all_white, sizeof(all_white));
    cm_display_send_command(0x02, 0x07);
    cm_delay_ms(500);

    /* Push frames. */
    cm_display_push_frame_data(sbuca_logo_vertical_frame_data);
    cm_delay_ms(500);
    cm_display_power_off();

    sei();

    /* Idle loop - Don't put anything here.:) */
    while(1) {
    }

    return 0;
}
