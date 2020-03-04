/**
 * @ingroup     boards_atsamr21b18-mz210pa
 * @{
 *
 * @file
 * @brief       Board specific implementations for the Atmel SAM R21 MZ210PA
 *              module, based on SAM R21 Xplained Pro board definition
 *
 * @author      Samuel Kendall <sjk2g17@soton.ac.uk>
 *
 * @}
 */

#include "board.h"
#include "periph/gpio.h"

/* Only 1 antenna on module
void board_antenna_config(uint8_t antenna)
{
    if (antenna == RFCTL_ANTENNA_EXT){
        gpio_set(RFCTL1_PIN);
        gpio_clear(RFCTL2_PIN);
    }
    else if (antenna == RFCTL_ANTENNA_BOARD){
        gpio_clear(RFCTL1_PIN);
        gpio_set(RFCTL2_PIN);
    }
}
*/

void board_init(void)
{
    /* initialize the on-board LED */
    //pio_init(LED0_PIN, GPIO_OUT);  // There is no LED on this 

    /* initialize the on-board antenna switch */
    //gpio_init(RFCTL1_PIN, GPIO_OUT);
    //gpio_init(RFCTL2_PIN, GPIO_OUT);
    /* set default antenna switch configuration */
    //board_antenna_config(RFCTL_ANTENNA_DEFAULT);

    /* initialize the CPU */
    cpu_init();
}
