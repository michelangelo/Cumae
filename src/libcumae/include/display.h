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
#ifndef CM_DISPLAY_H
#define CM_DISPLAY_H

#include <stdint.h>
#include <stddef.h>

#include <cumae/base.h>

/*
 * The type used for time measurements in the Cumae
 * Display context.
 */
typedef uint16_t cm_time_t;

/*
 * Descriptor of the currently supported display panels.
 * As of now we only support the 1.44" variant and, as such,
 * some of the code is still using hardcoded values.
 */
enum cm_display_type_e { CM_DISPLAY_UNKNOWN = 0, CM_DISPLAY_144 };
typedef enum cm_display_type_e cm_display_type_t;

/*
 * Cumae Display setup structure.
 */
struct cm_display_context_s {
    cm_display_type_t type;
    cm_byte_t columns;
    cm_byte_t lines;
    cm_byte_t *line_buf;
    size_t line_buf_len;
    cm_time_t stage_time_ms;
    cm_byte_t tf;
    cm_byte_t ghost_iter;
    struct cm_display_callback_s *cb;
};

/*
 * Callbacks
 */
struct cm_display_callback_s {
    void (* cm_display_stage_updated)(const cm_byte_t *prev, const cm_byte_t* cur);
    void (* cm_display_ready)(void);
    void (* cm_display_error)(const cm_err_t);
};

/*
 * Initializes the Cumae Display context with the provided
 * setup structure.
 */
extern cm_err_t cm_display_init(const struct cm_display_context_s *);

/*
 * Sets up a Cumae display context with default values.
 * This is the preferred way to get all the necessary information
 * for a display.
 *
 * The first parameter is the desired type of display.
 */
extern cm_err_t cm_display_get_default_context(cm_display_type_t t,
                                               struct cm_display_context_s *ctx);

/*
 * SPI single command helper ("headerized").
 *
 * Use this function to send one command index/data byte.
 * This function does NOT expand o cm_display_context_spi_write() to
 * avoid wasting .text space.
 *
 * First parameter is the command index.
 */
extern void cm_display_send_command(cm_byte_t, cm_byte_t);

/*
 * SPI "dumb" SPI transfer function.
 *
 * Returns the last byte read in the SPI data register; as this is
 * gonna be used mostly only for the display power-up sequence it's
 * not inlined, hence the last byte in the SPI data register is always
 * gonna be 0x12 (or whatever).
 */
extern cm_byte_t cm_display_context_spi_xfer(cm_byte_t *, size_t);

/*
 * SPI "headerized" SPI write, aimed to send long data buffers.
 *
 * The first argument is the Command Index, followed by the actual
 * array of data bytes of size_t.
 */
extern void cm_display_send_data(cm_byte_t, cm_byte_t *, size_t);

/*
 * SPI "headerized" SPI read.
 *
 * Only the command index is needed; the SPI envelope is built
 * with dummy 0x00 bytes.
 */
extern cm_byte_t cm_display_context_spi_read(cm_byte_t);

/*
 * Powers up the display.
 */
extern void cm_display_power_up(void);

/*
 * Amazingly enough, powers off the display.;)
 */
extern void cm_display_power_off(void);

/*
 * Push the specified frame data structure to the
 * display controller.
 *
 * Please note: the  amount of data (size of the
 * array) is defined as a compile time define in
 * the display.c file. This will change in a future
 * release.
 */
extern void cm_display_push_frame_data(const cm_byte_t *);

/*
 * Performs a staged update of the display. The first
 * parameter (previous) should match whatever is on the
 * display at the moment of the calling.
 *
 * The first parameter is the pointer to the PREVIOUS frame.
 * The second parameter is the pointer to the NEXT frame.
 */
extern void cm_display_stage_update(const cm_byte_t *,
                                    const cm_byte_t *);

#endif /* CM_DISPLAY_H */
