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

//#include "periph/rtt.h"
//#include "periph/pm.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"

#include "si70xx_params.h"
#include "si70xx.h"
#define SENSOR_POWER_PIN GPIO_PIN(PA, 28)

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];

/* import "ifconfig" shell command, used for printing addresses */
extern int _gnrc_netif_config(int argc, char **argv);

/**/
char nanocoap_thread_stack[THREAD_STACKSIZE_MAIN];
void *nanocoap_thread(void *arg) {
    (void) arg;

    /* initialise nanocoap server */
    uint8_t buf[COAP_INBUF_SIZE];
    sock_udp_ep_t local = { .port=COAP_PORT, .family=AF_INET6 };
    nanocoap_server(&local, buf, sizeof(buf));

    /* should never return */
    return NULL;
}

/* global object for si70xx sensor */
si70xx_t dev;
int get_temperature(int argc, char **argv);
int get_humidity(int argc, char **argv);

int toggle_led(int argc, char **argv) {
    (void)argc;
    (void)argv;

    LED0_TOGGLE;
    return 0;
}

static const shell_command_t shell_commands[] = {
    { "get_temp", "gets temperature in celcius", get_temperature },
    { "get_humid", "gets relative humidity", get_humidity },
    { "toggle_led", "toggles LED", toggle_led},
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("RIOT nanocoap example application with shell and ds18 temperature sensor");
    /* turn on the temp sensor and init power toggle pin */
    gpio_init(SENSOR_POWER_PIN, GPIO_OUT); 
    gpio_set(SENSOR_POWER_PIN);

    /* initialise the temperature sensor */
    if(si70xx_init(&dev, &si70xx_params[0]) == 0) {
        puts("[OK]");
    }
    else puts("[ERROR]");

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* initialize nanocoap server instance by starting thread */
    thread_create(nanocoap_thread_stack, sizeof(nanocoap_thread_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                  nanocoap_thread, NULL, "nanocoap_thread");

    /* initialize shell on board */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);


    /* should be never reached */
    return 0;
}


int get_temperature(int argc, char **argv) {
    (void)argc;
    (void)argv;
    int16_t temperature;
    puts("Getting temp...\n");

    /* get temp in celsius */
    temperature = si70xx_get_temperature(&dev);
    printf("temperature: %d.%02d C\n", temperature / 100, temperature % 100);

    return 0;
}

int get_humidity(int argc, char **argv) {
    (void)argc;
    (void)argv;
    uint16_t humid;
    puts("Getting humidity...\n");

    /* get temp in celsius */
    humid = si70xx_get_relative_humidity(&dev);
    printf("relative humidity: %d.%02d\n", humid / 100, humid % 100);

    return 0;
}

