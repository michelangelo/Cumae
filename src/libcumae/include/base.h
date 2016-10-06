#ifndef cm_BASE_h
#define cm_BASE_h

#include <stdint.h>
#include <util/delay.h>

/*
 * This takes care of formatting the current version.
 */
#ifndef cm_GIT_HASH
#define cm_GIT_HASH "unversioned"
#endif
#define cm_VERSION "0.4.0 (" cm_GIT_HASH ")"

/*
 * We keep things simple by having only a bunch of print
 * levels. They are pretty clear, though.:)
 */
enum cm_print_level_e { NONE = 0, ERROR, DEBUG, NORMAL };
typedef enum cm_print_level_e cm_print_level_t;

/* A simple byte. */
typedef uint8_t cm_byte_t;

enum cm_err_e { ERR_NONE, 
    ERR_UNSUPPORTED = 125,
    ERR_UNKNOWN = 126 };
typedef enum cm_err_e cm_err_t;

/*
 * Macros for pretty-debugging.
 */
#define P(x) cm_print(NORMAL, x);
#define D(x) cm_print(DEBUG, x);
#define E(x) cm_print(ERROR, x);

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
 * Cumae aliases for common functions.
 */
#define cm_delay_ms(m) _delay_ms(m)

#endif /* cm_BASE_h */
