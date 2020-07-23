/**
 * @ingroup     boards_3yp-node
 * @{
 *
 * @file        board.c
 * @brief       Board specific implementations for the SAM R30 based 3YP node
 *
 * @author      Samuel Kendall <sjk2g17@soton.ac.uk>
 *
 * @}
 */

#include <stdio.h>

#include "board.h"
#include "cpu.h"
#include "periph/gpio.h"

void led_init(void);
void power_init(void);

void board_init(void)
{
    /* initialize the CPU */
    cpu_init();

    /* initialize the boards LEDs */
    led_init();
    
    /* initialize load switch */
    power_init();
}


/**
 * @brief Initialize the boards on-board LED
 */
void led_init(void)
{
    gpio_init(LED0_PIN, GPIO_OUT);
    gpio_set(LED0_PIN); /* gpio is inverted => clear */
}

void power_init(void)
{
    gpio_init(SENSOR_POWER_PIN, GPIO_OUT);
    gpio_set(SENSOR_POWER_PIN); /* gpio not inverted */
}

