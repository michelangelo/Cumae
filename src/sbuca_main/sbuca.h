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
#ifndef SBUCA_H
#define SBUCA_H

#define FORCE_RESET_DISPLAY(); cm_display_power_up();\
    cm_display_send_data(0x0A, all_white, sizeof(all_white));\
    cm_display_send_command(0x02, 0x07);\
    _delay_ms(500);\
    cm_display_send_data(0x0A, all_black, sizeof(all_black));\
    cm_display_send_command(0x02, 0x07);\
    _delay_ms(500);\
    cm_display_send_data(0x0A, all_white, sizeof(all_white));\
    cm_display_send_command(0x02, 0x07);\
    _delay_ms(500);\
    cm_display_power_off();

#define PUSH_SBUCA_FRAME(f); cm_display_power_up();\
    cm_display_push_frame_data(f);\
    _delay_ms(500);\
    cm_display_power_off();

#define FAST_BLINK_STATUS_LED(n); uint8_t bsl_cnt = 0;\
    for(;bsl_cnt < n; ++bsl_cnt) {\
        PORTB |= 1 << PB1;\
        _delay_ms(80);\
        PORTB &= ~(1 << PB1);\
        _delay_ms(80); }\

#endif /* SBUCA_H */
