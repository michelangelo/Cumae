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
#include <avr/sleep.h>

#include <cumae/base.h>
#include <cumae/display.h>
#include <cumae/version.h>

#include <frames/sbuca_logo_vertical.h>
#include <frames/firefox.h>
#include <frames/freebsd_vertical.h>
#include <frames/gnuhead_vertical.h>
#include <frames/eff_vertical.h>
#include <frames/lowpower_vertical.h>

#include "def_frames.h"

/*** Function declarations ***/
void sbuca_frame_updated(const cm_byte_t *, const cm_byte_t *);
void sbuca_display_error(const cm_err_t);

/*** Variable declarations ***/

/*
 * A "has it all" structure for all the SBuCa related variables
 * and data.
 */
struct sbuca_info_s {
    uint16_t lp_tick_counter;
    uint8_t cur_frame_id;
};
static struct sbuca_info_s sbuca_info;

/*
 * These are the needed structs for the SBuCa display to be
 * correctly initialized.
 */
struct cm_display_context_s sbuca_144;
struct cm_display_callback_s sbuca_cb = {
    .cm_display_stage_updated = &sbuca_frame_updated,
    .cm_display_error = &sbuca_display_error
};

/*** Text ***/

/*
 * INT0 is bound to SW_PAGE_INT (aka: "page turn" interrupt).
 */
ISR(INT0_vect, ISR_BLOCK)
{
    /*
     * Disable the Power-Down SE bit that is set right before going to
     * sleep.
     */
    sbuca_info.lp_tick_counter = 0; /* Reset LP tick counter. */
    cm_display_power_up();

    /* Loop over the specified frames. */
    switch(sbuca_info.cur_frame_id) {
        case 0:
            cm_display_stage_update(sbuca_logo_vertical_frame_data, gnuhead_vertical_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 1:
            cm_display_stage_update(gnuhead_vertical_frame_data, eff_vertical_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 2:
            cm_display_stage_update(eff_vertical_frame_data, freebsd_vertical_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 3:
            cm_display_stage_update(freebsd_vertical_frame_data, firefox_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 4:
            cm_display_stage_update(firefox_frame_data, sbuca_logo_vertical_frame_data);
            sbuca_info.cur_frame_id = 0;
            break;
    }

    cm_display_power_off();
}

/*
 * When Timer1 overflows it's time to check if we're due into
 * low-power mode.
 */
ISR(TIMER1_OVF_vect, ISR_BLOCK)
{
    /* Timer1 should overflow twice every second. */
    sbuca_info.lp_tick_counter++;

    if (sbuca_info.lp_tick_counter == 60) {
        P("*** Low-Power Mode");
        sbuca_info.lp_tick_counter = 0;

        /* Prepare for Power-Down mode. */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sleep_cpu();
    }
}

void sbuca_frame_updated(const cm_byte_t *p, const cm_byte_t *c) {
    P("*** Frame updated");
}

void sbuca_display_error(const cm_err_t err) {
    printf("!!! Display Error, code: %d\r\n", err);
}

int main()
{
    cm_usart_init();

    printf("\r\n*** Cumae %s (build date: %s).\r\n", cm_ver_git_hash, cm_ver_build_date);
    printf("*** Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>\r\n\r\n");

    /* STATUS_LED (PB1) as output, ON for OK. */
    DDRB |= 1 << PB1;
    PORTB |= 1 << PB1;

    /* Enable interrupts on INT0 on rising signal. */
    EIMSK = 0x1;
    EICRA = 0x3;

    /* Timer1 (16-bit) overflow counter enabled with a /64 prescaler. */
    TCCR1B |= 0x3; /* Prescaler */
    TIMSK1 |= 0x1; /* Interrupt */

    /* Initialize the display context. */
    cm_display_get_default_context(CM_DISPLAY_144, &sbuca_144);
    sbuca_144.stage_time_ms = 40;
    sbuca_144.cb = &sbuca_cb;

    cm_display_init(&sbuca_144);

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
