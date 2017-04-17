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
#ifndef CM_BASE_H
#define CM_BASE_H

#include <stdint.h>
#include <util/delay.h>

#define CM_ATTR_PACKED __attribute__ ((__packed__))
#define CM_PRETTY_UUID_T_LEN 37

/*
 * We keep things simple by having only a bunch of print
 * levels. They are pretty clear, though.:)
 */
enum cm_print_level_e { NONE = 0, ERROR, DEBUG, NORMAL };
typedef enum cm_print_level_e cm_print_level_t;

/* Basic type aliases. */
typedef uint8_t cm_byte_t;
typedef uint16_t cm_word_t;

/* To be used as backing store for printable UUIDs. */
typedef struct cm_pretty_uuid_t {
    char uuid[CM_PRETTY_UUID_T_LEN];
} cm_pretty_uuid_t;


/*
 * Cumae base error type.
 * Non-positive numbers identify errors.
 */
enum cm_err_e {
    ENONE = 0,
    ENODEVICE = 123,
    ENOMEM = 124,
    EUNSUPPORTED = 125,
    ENOTREADY = 126,
    EUNKNOWN = 254
};
typedef enum cm_err_e cm_err_t;

/*
 * Representation of a UUID, commonly used as device signature.
 */
struct CM_ATTR_PACKED cm_uuid_s {
    cm_byte_t time_low[4];
    cm_byte_t time_mid[2];
    cm_byte_t time_hi_and_version[2];
    cm_byte_t clock_seq_hi_res;
    cm_byte_t clock_seq_lo;
    cm_byte_t node[6];
};
typedef struct cm_uuid_s cm_uuid_t;

/*
 * Device Permanent Data.
 *
 * This structure is meant to be persistent (on EEPROM); consumers
 * of this structure can choose to append their own data to it.
 */
struct CM_ATTR_PACKED cm_permanent_data_s {
    cm_byte_t version;  /* Structure version (starting from 1).  */
    cm_uuid_t uuid;     /* UUID of the device.                   */
    cm_byte_t padding;  /* Align/padding; do whatever you like.;)*/
    cm_word_t data_len; /* Size of the following data content.   */
    void *data;         /* Free to use.                          */
};

#if 0
/* A Cumae context. */
struct cm_context_s {
    struct cm_context_cb_s *cb;
};

/*
 * A Cumae context may have a set of associated
 * callbacks.
 */
struct cm_context_cb_s {
    (void)(* cm_context_initialized)(void);
};
#endif

/*
 * Macros for pretty-debugging.
 */
#define P(x) cm_print(NORMAL, x);
#define D(x) cm_print(DEBUG, x);
#define E(x) cm_print(ERROR, x);

/*
 * Initializes a Cumae context.
 *
 * Not all the functions in Cumae are bound to a context.
 */
#if 0
extern cm_err_t cm_init(struct cm_context_s *);
#endif

/*
 * This initializes the USART port with the specified
 * 'BAUD' baudrate, 8 bit data, 1 bit stop and no
 * flow-control.
 *
 * If 'BAUD' has not been specified at compile time,
 * it defaults to 9600.
 *
 * This also redirects stdout on the USART for your
 * printf() convenience.:)
 */
extern void cm_usart_init(void);

/*
 * Transmits one byte via USART.
 * Returns whatever is in the UDR buffer.
 *
 * Of course, the USART should be initialized.;)
 */
extern cm_byte_t cm_usart_tx(const cm_byte_t);

/*
 * Initialized the SPI controller.
 */
extern void cm_spi_init(void);

/*
 * This is a wrapper to write one byte in the SPI data register.
 *
 * TODO: macro here?
 */
extern cm_byte_t cm_spi_w1r1(cm_byte_t data);

/*
 * Print function wrapper.
 *
 * TODO: context level for filters
 * TODO: timestamp
 * TODO: ellipsis
 */
extern void cm_print(cm_print_level_t, char *);

/*
 * Populates the parameter with the content of the permanent
 * data stored within the device.
 */
extern cm_err_t cm_get_permanent_data(struct cm_permanent_data_s *);

/*
 * Pretty-format the provided cm_uuid_t into the canonical
 * 8-4-4-4-12 format suitable for printing.
 */
extern void cm_pretty_print_uuid(cm_pretty_uuid_t *, const cm_uuid_t *);

/*
 * Delay 'double' milliseconds.
 */
extern void cm_delay_ms(uint16_t);

#endif /* CM_BASE_H */
