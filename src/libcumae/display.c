#include <cumae/base.h>
#include <cumae/display.h>

#include <avr/io.h>
#include <stdio.h>

void cumae_display_send_command(uint8_t idx, uint8_t dat)
{
    PORTB &= ~(1 << PB2);
    cumae_spi_w1r1(0x70);
    cumae_spi_w1r1(idx);
    PORTB |= 1 << PB2;

    cumae_delay_ms(1);

    PORTB &= ~(1 << PB2);
    cumae_spi_w1r1(0x72);
    cumae_spi_w1r1(dat);
    PORTB |= 1 << PB2;
}

uint8_t cumae_display_spi_xfer(uint8_t *data, size_t len)
{
    PORTB &= ~(1 << PB2);

    size_t biter = 0;
    uint8_t tempdata = 0;
    for (; biter < len;  ++biter)
        tempdata = cumae_spi_w1r1(data[biter]);

    PORTB |= (1 << PB2);

    return tempdata;
}

inline void cumae_display_send_data(uint8_t idx,
                                    uint8_t *data,
                                    size_t data_len)
{
    PORTB &= ~(1 << PB2);

    cumae_spi_w1r1(0x70);
    cumae_spi_w1r1(idx);

    PORTB |= (1 << PB2);
    cumae_delay_ms(1);
    PORTB &= ~(1 << PB2);

    cumae_spi_w1r1(0x72);

    size_t biter;
    for(biter = 0; biter < data_len; ++biter)
        cumae_spi_w1r1(data[biter]);

    PORTB |= (1 << PB2);
}

inline uint8_t cumae_display_spi_read(uint8_t c_idx)
{
    PORTB &= ~(1 << PB2);

    cumae_spi_w1r1(0x70);
    cumae_spi_w1r1(c_idx);

    PORTB |= (1 << PB2);
    cumae_delay_ms(1);
    PORTB &= ~(1 << PB2);

    cumae_spi_w1r1(0x73);
    uint8_t read_data = cumae_spi_w1r1(0x00);

    PORTB |= (1 << PB2);

    return read_data;
}

void cumae_display_power_up(void)
{
    uint8_t pseq[] = { 0x71, 0x00 };

    /* BUSY as INPUT */
    DDRB &= ~(1 << PB0);

    /* RSTN as OUTPUT, init to low. */
    DDRD |= 1 << PD7;
    PORTD &= ~(1 << PD7);

    /* Display Vcc/Vdd as OUTPUT, init to low. */
    DDRD |= 1 << PD4;
    PORTD &= ~(1 << PD4);

    /* DCHARGE as OUTPUT, init to low. */
    DDRD |= 1 << PD6;
    PORTD &= ~(1 << PD6);

    /* CS as OUTPUT, init to low. */
    DDRB |= 1 << PB2;
    PORTB &= ~(1  << PB2);

    cumae_spi_init();

    cumae_delay_ms(6);

    /* Vcc/Vdd up. */
    PORTD |= 1 << PD4;
    cumae_delay_ms(11);

    /* RSTN and SS up. */
    PORTD |= 1 << PD7;
    PORTB |= 1 << PB2;
    cumae_delay_ms(6);

    /* RSTN low. */
    PORTD &= ~(1 << PD7);
    cumae_delay_ms(6);

    /* RSTN up. */
    PORTD |= 1 << PD7;
    cumae_delay_ms(6);

    while ((PINB & PB0) == 1) {
        D("Waiting...\r\n");
        cumae_delay_ms(10);
    }

    cumae_delay_ms(6);
    if (cumae_display_spi_xfer(pseq, 2) != 0x12) {
        E("G2 driver not detected; defaulting.");
        while(1) {}
    }

    D("G2 driver detected.");

    /* Disable OE. */
    cumae_display_send_command(0x02, 0x40);
    if ((cumae_display_spi_read(0x0F) & 0x80) != 0x80) {
        E("Breakage check failed; defaulting.");
        while(1) {}
    }

    /* Power saving mode. */
    cumae_display_send_command(0x0B, 0x02);

    /* Channel data. */
    uint8_t cmd_idx_cdata[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x00 };
    cumae_display_send_data(0x01, cmd_idx_cdata, 8);

    /* High power mode. */
    cumae_display_send_command(0x07, 0xD1);

    /* Power setting. */
    cumae_display_send_command(0x08, 0x02);

    /* Vcom level. */
    cumae_display_send_command(0x09, 0xC2);

    /* Power setting. */
    cumae_display_send_command(0x04, 0x03);

    /* Driver latch on. */
    cumae_display_send_command(0x03, 0x01);

    /* Driver latch off. */
    cumae_display_send_command(0x03, 0x00);

    cumae_delay_ms(6);

    uint8_t charge_pump;
    for(charge_pump = 0; charge_pump < 4; ++charge_pump) {

        /* Start chargepump positive voltage. */
        cumae_display_send_command(0x05, 0x01);
        cumae_delay_ms(155);

        /* Start chargepump negative voltage. */
        cumae_display_send_command(0x05, 0x03);
        cumae_delay_ms(95);

        /* Start chargepump Vcom on. */
        cumae_display_send_command(0x05, 0x0F);

        cumae_delay_ms(45);

        if ((cumae_display_spi_read(0x0F) & 0x40) == 0x40) {

            /* Output enable to disable. */
            cumae_display_send_command(0x02, 0x06);
            break;
        }

        charge_pump++;
    }
}

void cumae_display_power_off(void)
{
    /*
     * To save bandwidth we write ONE nothing line but
     * scan it all over the 96 lines.
     */
    uint8_t line_nothing[] = {
        /* Nothing Data (16 bytes). */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        /* Scan everywhere (24 bytes). */
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0xFF,

        /* Nothing Data (16 bytes). */
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,

        0x00 /* Border byte. */
    };

    uint8_t border_dummy_line[] = {
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0xAA /* Dummy border. */
    };

    /*
     * This writes one "virtual" nothing frame.
     * It sends one line of nothing with all scan bytes active.
     */
    cumae_display_send_data(0x0A, line_nothing, 73);
    cumae_display_send_command(0x02, 0x07);

    /* Write border dummy line. */
    cumae_display_send_data(0x0A, border_dummy_line, 73);
    cumae_display_send_command(0x02, 0x07);

    cumae_delay_ms(200);

    /* Power saving mode? */
    cumae_display_send_command(0x0B, 0x00);

    /* Latch reset turn on. */
    cumae_display_send_command(0x03, 0x01);

    /* Power off chargepump. */
    cumae_display_send_command(0x05, 0x03);

    /* Power off chargepump negative voltage. */
    cumae_display_send_command(0x05, 0x01);

    /* Discharge internal. */
    cumae_display_send_command(0x04, 0x80);

    /* Power off chargepump positive voltage. */
    cumae_display_send_command(0x05, 0x00);

    /* Turn off oscillator. */
    cumae_display_send_command(0x07, 0x01);

    cumae_delay_ms(55);

    /*
     * TODO: Turn off SPI here.
     */
}
