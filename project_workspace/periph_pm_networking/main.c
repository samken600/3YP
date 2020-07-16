/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       CoAP example server application (using nanocoap)
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <stdio.h>

#include "periph/rtt.h"
#include "pm_layered.h"
#include "xtimer.h"
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"
#include "net/gnrc/netif.h"

#define SENSOR_POWER_PIN GPIO_PIN(PA, 28)

#define PHYSICAL_NETIF 5

#define MAIN_QUEUE_SIZE     (8)

#ifndef BTN0_INT_FLANK
#define BTN0_INT_FLANK GPIO_RISING
#endif

static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

int toggle_led(int argc, char **argv) {
    (void)argc;
    (void)argv;

    LED0_TOGGLE;
    return 0;
}

void cb(void *arg) {
    (void)arg;
    puts("RTT Callback");
}

#if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
static void btn_cb(void *ctx) {
    (void)ctx;
    puts("BTN0 pressed.");
}
#endif

void radio_off(gnrc_netif_t *netif) {
    netopt_state_t devstate = NETOPT_STATE_SLEEP;
    netif->dev->driver->set(netif->dev,
                            NETOPT_STATE,
                            &devstate,
                            sizeof(devstate));
}

void radio_on(gnrc_netif_t *netif) {
    netopt_state_t devstate = NETOPT_STATE_IDLE;
    netif->dev->driver->set(netif->dev,
                            NETOPT_STATE,
                            &devstate,
                            sizeof(devstate));
}

int cmd_radio_off(int argc, char **argv) {
    (void)argc;
    (void)argv;
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(netif == NULL) {
        printf("Is at86rf2xx on pid %d?", PHYSICAL_NETIF);
        return 1;
    }
    radio_off(netif);
    return 0;
}

int cmd_radio_on(int argc, char **argv) {
    (void)argc;
    (void)argv;
    gnrc_netif_t *netif = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(netif == NULL) {
        printf("Is at86rf2xx on pid %d?", PHYSICAL_NETIF);
        return 1;
    }
    radio_on(netif);
    return 0;
}

int off(int argc, char **argv) {
    int power_mode = 0;
    if(argc == 2) {
        power_mode = atoi(argv[1]);
    }

    gnrc_netif_t *netif = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(netif == NULL) {
        printf("Is at86rf2xx on pid %d?", PHYSICAL_NETIF);
        return 1;
    }
    
    radio_off(netif);
    puts("Radio is sleeping...");

    uint32_t start = rtt_get_counter();
    rtt_set_alarm((start + 10 * RTT_FREQUENCY) & RTT_MAX_VALUE, cb, 0);
    puts("Alarm set, sleeping");
    
    printf("Power Mode is: %d\n", power_mode);

    pm_set(power_mode);
    
    puts("Woken...");
    
    xtimer_sleep(1);

    puts("Timer works");
    radio_on(netif);
    return 0;
}

int set_rtt(int argc, char **argv) {
    (void)argc;
    (void)argv;

    uint32_t start = rtt_get_counter();
    printf("setting rtt, start=%ld end=%ld", start, ((start + 10 * RTT_FREQUENCY) & RTT_MAX_VALUE));
    rtt_set_alarm((start + 10 * RTT_FREQUENCY) & RTT_MAX_VALUE, cb, 0);

    return 0;
}

static const shell_command_t shell_commands[] = {
    { "toggle_led", "toggles LED", toggle_led },
    { "off", "waits 10 second", off },
    { "radio_off", "turn radio off", cmd_radio_off },
    { "radio_on", "turn radio on", cmd_radio_on },
    { "set", "tests 10 sec rtt", set_rtt },
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("RIOT nanocoap example application with shell and ds18 temperature sensor");

    /* init rtt */
    rtt_init();

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    /* turn off the temp sensor and init power toggle pin */
    gpio_init(SENSOR_POWER_PIN, GPIO_OUT); 
    gpio_clear(SENSOR_POWER_PIN);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");

    /* enable low power cache mode */
    NVMCTRL->CTRLB.bit.READMODE = NVMCTRL_CTRLB_READMODE_LOW_POWER_Val;


    gnrc_netif_t *netif = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(netif == NULL) {
        printf("Is at86rf2xx on pid %d?", PHYSICAL_NETIF);
        return 1;
    }
    radio_off(netif);
    puts("Radio is off");

    #if defined(MODULE_PERIPH_GPIO_IRQ) && defined(BTN0_PIN)
    puts("using BTN0 as wake-up source");
    gpio_init_int(BTN0_PIN, BTN0_MODE, BTN0_INT_FLANK, btn_cb, NULL);
    #endif

    /* initialize shell on board */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    /* should be never reached */
    return 0;
}

