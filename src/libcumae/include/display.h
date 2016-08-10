#ifndef CUMAE_DISPLAY_H
#define CUMAE_DISPLAY_H

#include <stdint.h>
#include <stddef.h>

/*
 * SPI single command helper ("headerized").
 *
 * Use this function to send one command index/data byte.
 * This function does NOT expand o cumae_display_spi_write() to
 * avoid wasting .text space.
 *
 * First parameter is the command index.
 */
extern void cumae_display_send_command(uint8_t, uint8_t);

/*
 * SPI "dumb" SPI transfer function.
 *
 * Returns the last byte read in the SPI data register; as this is
 * gonna be used mostly only for the display power-up sequence it's
 * not inlined, hence the last byte in the SPI data register is always
 * gonna be 0x12 (or whatever).
 */
extern uint8_t cumae_display_spi_xfer(uint8_t *, size_t);

/*
 * SPI "headerized" SPI write, aimed to send long data buffers.
 *
 * The first argument is the Command Index, followed by the actual
 * array of data bytes of size_t.
 */
extern void cumae_display_send_data(uint8_t, uint8_t *, size_t);

/*
 * SPI "headerized" SPI read.
 *
 * Only the command index is needed; the SPI envelope is built
 * with dummy 0x00 bytes.
 */
extern uint8_t cumae_display_spi_read(uint8_t);

/*
 * Powers up the display.
 */
extern void cumae_display_power_up(void);

/*
 * Amazingly enough, powers off the display.;)
 */
extern void cumae_display_power_off(void);

#endif /* CUMAE_DISPLAY_H */
