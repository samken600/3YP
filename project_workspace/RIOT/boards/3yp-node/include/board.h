/**
 * @defgroup    boards_3yp-node Atmel SAM R30 based node
 * @ingroup     boards
 * @brief       Support for the Atmel SAM R30 Xplained Pro board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Atmel SAM R30 Xplained Pro board.
 *
 * @author      Samuel Kendall <sjk2g17@soton.ac.uk>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name NG_AT86RF212B configuration
 * @{
 */
#define AT86RF2XX_PARAM_SPI         SPI_DEV(0)
#define AT86RF2XX_PARAM_CS          GPIO_PIN(PB, 31)
#define AT86RF2XX_PARAM_INT         GPIO_PIN(PB, 0)
#define AT86RF2XX_PARAM_SLEEP       GPIO_PIN(PA, 20)
#define AT86RF2XX_PARAM_RESET       GPIO_PIN(PB, 15)
#define AT86RF2XX_PARAM_SPI_CLK     SPI_CLK_5MHZ
/** @}*/

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED_PORT                    PORT->Group[0]

#define LED0_PIN                    GPIO_PIN(PA, 14)
#define LED0_MASK                   (1 << 14)
#define LED0_ON                     (LED_PORT.OUTCLR.reg = LED0_MASK)
#define LED0_OFF                    (LED_PORT.OUTSET.reg = LED0_MASK)
#define LED0_TOGGLE                 (LED_PORT.OUTTGL.reg = LED0_MASK)
/** @} */

/**
 * @name    BTN0 (SW0 Button) pin definitions
 * @{
 */
#define BTN0_PORT                   PORT->Group[0]
#define BTN0_PIN                    GPIO_PIN(PA, 7)
#define BTN0_MODE                   GPIO_IN_PU
/** @} */

/**
 * @name    Power pin definitions for load switch
 * @{
 */
#define SENSOR_POWER_PORT           PORT->Group[0]
#define SENSOR_POWER_PIN            GPIO_PIN(PA, 28)
#define SENSOR_POWER_MASK           (1 << 28)

#define SENSOR_POWER_ON             (SENSOR_POWER_PORT.OUTSET.reg = SENSOR_POWER_MASK)
#define SENSOR_POWER_OFF            (SENSOR_POWER_PORT.OUTCLR.reg = SENSOR_POWER_MASK)
#define SENSOR_POWER_TOGGLE         (SENSOR_POWER_PORT.OUTTGL.reg = SENSOR_POWER_MASK)
/** @} */

/**
 * @name MCP9808 configuration. Uses standard JC42 interface
 * @{
 */
#define JC42_PARAM_I2C_DEV          I2C_DEV(0)
#define JC42_PARAM_ADDR             (0x19)
#define JC42_PARAM_SPEED            I2C_SPEED_FAST
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
