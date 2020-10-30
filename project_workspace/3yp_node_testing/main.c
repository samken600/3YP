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
#include "periph/pm.h"
//#include "net/nanocoap_sock.h"
#include "xtimer.h"
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"
#include "net/gnrc/netif.h"

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
/* extern int _gnrc_netif_config(int argc, char **argv); */
extern gnrc_netif_t *lwmac_netif;

void _gnrc_lwmac_set_netdev_state(gnrc_netif_t *netif, netopt_state_t devstate);

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


int off(int argc, char **argv) {
    int power_mode = 0;
    if(argc == 2) {
        int power_mode = atoi(argv[1]);
        printf("Power Mode is: %d\n", power_mode);
    }

    gnrc_netif_t *netif = gnrc_netif_get_by_pid(7);
    if(netif == NULL) {
        puts("Is at86rf2xx on pid 7?");
        return 1;
    }
    
    radio_off(netif);
    puts("Radio is sleeping...");

    uint32_t start = rtt_get_counter();
    rtt_set_alarm((start + 10 * RTT_FREQUENCY) & RTT_MAX_VALUE, cb, 0);
    puts("Alarm set, sleeping");

    pm_set(power_mode);
    
    puts("Woken...");
    
    //xtimer_init();
    xtimer_sleep(1);

    puts("Timer works");
    radio_on(netif);
    //ps();
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "toggle_led", "toggles LED", toggle_led},
    { "off", "waits 10 second", off},
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("RIOT nanocoap example application with shell and ds18 temperature sensor");
    /* initialise the temperature sensor */
//    int16_t result;
//    result = ds18_init(&dev, &(ds18_params[0]));
//    if (result == DS18_ERROR) {
//        puts("[Error] The sensor could not be initialised");
//        return 1;
//    }
//    gpio_init(GPIO_PIN(PA, 28), GPIO_OUT);
//    gpio_set(GPIO_PIN(PA,28));

//    if(si70xx_init(&dev, &si70xx_params[0]) == 0) {
//        puts("[OK]");
//    }
//    else puts("[ERROR]");

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");

    /* initialize nanocoap server instance by starting thread */
//    thread_create(nanocoap_thread_stack, sizeof(nanocoap_thread_stack),
//                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
//                  nanocoap_thread, NULL, "nanocoap_thread");



    /* initialize shell on board */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);
    
    /* should be never reached */
    return 0;
}

