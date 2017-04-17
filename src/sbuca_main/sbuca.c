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

#include <frames/welcome_horizontal.h>
#include <frames/iam_horizontal.h>
#include <frames/openhardware_horizontal.h>
#include <frames/runningcumae_horizontal.h>
#include <frames/hackme_horizontal.h>

#include "def_frames.h"
#include "sbuca.h"

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
    uint8_t *current_frame;
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
ISR(INT0_vect)
{
    /*
     * Disable the Power-Down SE bit that is set right before going to
     * sleep.
     */
    sleep_disable();
    sbuca_info.lp_tick_counter = 0; /* Reset LP tick counter. */
    cm_display_power_up();

    /* Loop over the specified frames. */
    switch(sbuca_info.cur_frame_id) {
        case 0:
            cm_display_stage_update(sbuca_info.current_frame, iam_horizontal_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 1:
            cm_display_stage_update(sbuca_info.current_frame, openhardware_horizontal_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 2:
            cm_display_stage_update(sbuca_info.current_frame, runningcumae_horizontal_frame_data);
            sbuca_info.cur_frame_id++;
            break;
        case 3:
            cm_display_stage_update(sbuca_info.current_frame, welcome_horizontal_frame_data);
            sbuca_info.cur_frame_id = 0;
            break;
#if 0
        case 4:
            cm_display_stage_update(hackme_horizontal_frame_data, welcome_horizontal_frame_data);
            sbuca_info.cur_frame_id = 0;
            break;
#endif
    }

    cm_display_power_off();
}

/*
 * When Timer1 overflows it's time to check if we're due into
 * low-power mode.
 */
ISR(TIMER1_OVF_vect)
{
    /* Timer1 should overflow twice every second. */
    sbuca_info.lp_tick_counter++;

    if (sbuca_info.lp_tick_counter == 180) {
        printf("*** Entering Low-Power Mode...\r\n");
        sbuca_info.lp_tick_counter = 0;
        FAST_BLINK_STATUS_LED(5);

        /* Prepare to push the 'Sleep' frame. */
        FORCE_RESET_DISPLAY();
        PUSH_SBUCA_FRAME(hackme_horizontal_frame_data);
        sbuca_info.current_frame = (uint8_t *)hackme_horizontal_frame_data;

        /* Prepare for Power-Down mode. */
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();
        sei(); /* As interrupts are cli'd in a ISR_BLOCK, we enable them. */
        sleep_cpu();
    }
}

void sbuca_frame_updated(const cm_byte_t *p, const cm_byte_t *c)
{
    sbuca_info.current_frame = (uint8_t *)c;
    printf("*** Frame updated\r\n");
}

void sbuca_display_error(const cm_err_t err)
{
    printf("!!! Display Error, code: %d\r\n", err);
}

int main()
{
    cm_usart_init();

    printf("\r\n*** Cumae %s (build date: %s).\r\n", cm_ver_git_hash, cm_ver_build_date);
    printf("*** Copyright (c) 2016 Michelangelo De Simone <michel@ngelo.eu>\r\n\r\n");

    struct cm_permanent_data_s perm;
    cm_pretty_uuid_t u;

    cm_get_permanent_data(&perm);
    cm_pretty_print_uuid(&u, &perm.uuid);

    printf("*** Version : %d\r\n", perm.version);
    printf("*** UUID ...: %s\r\n", u.uuid);

    /* STATUS_LED (PB1) as output, blink for OK. */
    DDRB |= 1 << PB1;
    FAST_BLINK_STATUS_LED(3);

    /* Enable interrupts on INT0 on rising signal. */
    EIMSK = 0x1;
    EICRA = 0x3;

    /* Timer1 (16-bit) overflow counter enabled with a /64 prescaler. */
    TCCR1B |= 0x3; /* Prescaler */
    TIMSK1 |= 0x1; /* Interrupt */

    /* Initialize the display context. */
    cm_display_get_default_context(CM_DISPLAY_144, &sbuca_144);
    sbuca_144.stage_time_ms = 40; /* Faster page turns. */
    sbuca_144.cb = &sbuca_cb;
    cm_display_init(&sbuca_144);

    /* Display power up sequence. */
    FORCE_RESET_DISPLAY();

    /* Push intro frame. */
    PUSH_SBUCA_FRAME(welcome_horizontal_frame_data);
    sbuca_info.current_frame = (uint8_t *)welcome_horizontal_frame_data;

    /* Enable interrupts. */
    sei();

    /* Idle loop - Don't put anything here.:) */
    while(1) {
    }

    return 0;
}
