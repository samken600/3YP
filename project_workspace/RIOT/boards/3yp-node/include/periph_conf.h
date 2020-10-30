/*
 * Copyright (C) 2017 Baptiste Clenet <bapclenet@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_samr30-xpro
 * @{
 *
 * @file
 * @brief       Peripheral MCU configuration for the Atmel SAM R30 Xplained Pro board
 *
 * @author      Baptiste Clenet <bapclenet@gmail.com>
 */

#ifndef PERIPH_CONF_H
#define PERIPH_CONF_H

#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   GCLK reference speed
 */
#define CLOCK_CORECLOCK     (16000000U)

/**
 * @name    Timer peripheral configuration
 * @{
 */
static const tc32_conf_t timer_config[] = {
    {   /* Timer 0 - System Clock */
        .dev            = TC0,
        .irq            = TC0_IRQn,
        .mclk           = &MCLK->APBCMASK.reg,
        .mclk_mask      = MCLK_APBCMASK_TC0 | MCLK_APBCMASK_TC1,
        .gclk_id        = TC0_GCLK_ID,
//        .gclk_src       = SAM0_GCLK_MAIN,
        .gclk_src       = SAM0_GCLK_8MHZ,
        .flags          = TC_CTRLA_MODE_COUNT32,
    }
};

/* Timer 0 configuration */
#define TIMER_0_CHANNELS    2
#define TIMER_0_ISR         isr_tc0
#define TIMER_NUMOF         ARRAY_SIZE(timer_config)
/** @} */

/**
 * @name    UART configuration
 * @{
 */
static const uart_conf_t uart_config[] = {
    {    /* Virtual COM Port */
        .dev      = &SERCOM3->USART,
        .rx_pin   = GPIO_PIN(PA,25),
        .tx_pin   = GPIO_PIN(PA,24),
#ifdef MODULE_PERIPH_UART_HW_FC
        .rts_pin  = GPIO_UNDEF,
        .cts_pin  = GPIO_UNDEF,
#endif
        .mux      = GPIO_MUX_C,
        .rx_pad   = UART_PAD_RX_3,
        .tx_pad   = UART_PAD_TX_2,
        .flags    = UART_FLAG_NONE,
        .gclk_src = SAM0_GCLK_MAIN,
    }
};

/* interrupt function name mapping */
#define UART_0_ISR          isr_sercom3

#define UART_NUMOF          ARRAY_SIZE(uart_config)
/** @} */

/**
 * @name    SPI configuration
 * @{
 */
static const spi_conf_t spi_config[] = {
    {   /* Internal AT86RF212B */
        .dev      = &(SERCOM4->SPI),
        .miso_pin = GPIO_PIN(PC, 19),
        .mosi_pin = GPIO_PIN(PB, 30),
        .clk_pin  = GPIO_PIN(PC, 18),
        .miso_mux = GPIO_MUX_F,
        .mosi_mux = GPIO_MUX_F,
        .clk_mux  = GPIO_MUX_F,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        /* The SAML21 doesn't support DMA triggers on SERCOM5 (?) */
        .tx_trigger = DMA_TRIGGER_DISABLED,
        .rx_trigger = DMA_TRIGGER_DISABLED,
#endif
    },
    {   /* External SPI */
        .dev      = &(SERCOM1->SPI),
        .miso_pin = GPIO_PIN(PA, 16),
        .mosi_pin = GPIO_PIN(PA, 18),
        .clk_pin  = GPIO_PIN(PA, 19),
        .miso_mux = GPIO_MUX_C,
        .mosi_mux = GPIO_MUX_C,
        .clk_mux  = GPIO_MUX_C,
        .miso_pad = SPI_PAD_MISO_0,
        .mosi_pad = SPI_PAD_MOSI_2_SCK_3,
        .gclk_src = SAM0_GCLK_MAIN,
#ifdef MODULE_PERIPH_DMA
        /* The SAML21 doesn't support DMA triggers on SERCOM5 (?) */
        .tx_trigger = DMA_TRIGGER_DISABLED,
        .rx_trigger = DMA_TRIGGER_DISABLED,
#endif
    }
};

#define SPI_NUMOF           ARRAY_SIZE(spi_config)
/** @} */

/**
 * @name    I2C configuration
 * @{
 */
static const i2c_conf_t i2c_config[] = {
    {
        .dev      = &(SERCOM0->I2CM),
        .speed    = I2C_SPEED_NORMAL,
        .scl_pin  = GPIO_PIN(PA, 9),
        .sda_pin  = GPIO_PIN(PA, 8),
        .mux      = GPIO_MUX_C,
        .gclk_src = SAM0_GCLK_MAIN,
        .flags    = I2C_FLAG_NONE
     }
};
#define I2C_NUMOF          ARRAY_SIZE(i2c_config)
/** @} */

/**
 * @name    RTC configuration
 * @{
 */
#define EXTERNAL_OSC32_SOURCE                   0
#define INTERNAL_OSC32_SOURCE                   1
#define ULTRA_LOW_POWER_INTERNAL_OSC_SOURCE     0
/** @} */

/**
 * @name    RTT configuration
 * @{
 */
#ifndef RTT_FREQUENCY
#define RTT_FREQUENCY                           (32768U)
#endif
/** @} */

/**
 * @name ADC Configuration
 * @{
 */

/* ADC Default values */
#define ADC_PRESCALER                           ADC_CTRLB_PRESCALER_DIV256

#define ADC_NEG_INPUT                           ADC_INPUTCTRL_MUXNEG(0x18u)
#define ADC_REF_DEFAULT                         ADC_REFCTRL_REFSEL_INTVCC2

static const adc_conf_chan_t adc_channels[] = {
    /* port, pin, muxpos */
//    {GPIO_PIN(PA, 6), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN6)}, /* EXT1, pin 3 */
    {GPIO_PIN(PA, 7), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN7)},
//    {GPIO_PIN(PA, 10), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN18)},
//    {GPIO_PIN(PA, 11), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN19)},
//    {GPIO_PIN(PA, 2), ADC_INPUTCTRL_MUXPOS(ADC_INPUTCTRL_MUXPOS_AIN0)}
};

#define ADC_NUMOF                               ARRAY_SIZE(adc_channels)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CONF_H */
/** @} */
