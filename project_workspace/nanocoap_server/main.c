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

#include "net/nanocoap_sock.h"
#include "net/gnrc/rpl.h"
#include "xtimer.h"
#include "shell.h"

#define DS18_POWER_PIN GPIO_PIN(PA, 28)
#define DS18_PARAM_PIN GPIO_PIN(PA, 13)
#include "ds18.h"
#include "ds18_params.h"

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

/* global object for ds18 sensor */
ds18_t dev;
int get_temperature(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "get_temp", "gets temperature in celcius", get_temperature },
    { NULL, NULL, NULL }
};

int main(void)
{
    puts("RIOT nanocoap example application with shell and ds18 temperature sensor");

    /* initialise the temperature sensor */
    gpio_init(DS18_POWER_PIN, GPIO_OUT);
    gpio_set(DS18_POWER_PIN);
    int16_t result;
    result = ds18_init(&dev, &(ds18_params[0]));
    if (result == DS18_ERROR) {
        puts("[Error] The sensor could not be initialised");
        return 1;
    }
    gpio_clear(DS18_POWER_PIN);

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    /* print network addresses */
    puts("Configured network interfaces:");
    _gnrc_netif_config(0, NULL);
    gnrc_rpl_init(GNRC_RPL_DEFAULT_NETIF);

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
    gpio_set(DS18_POWER_PIN);
    if (ds18_get_temperature(&dev, &temperature) == DS18_OK) {
        puts("Got temp");
        bool negative = (temperature < 0);
        if (negative) {
            temperature = -temperature;
        }

    printf("Temperature [ºC]: %c%d.%02d"
           "\n+-------------------------------------+\n",
            negative ? '-': ' ',
           temperature / 100,
           temperature % 100);
    }
    else puts("Error");
    gpio_clear(DS18_POWER_PIN);
 
    return 0;
}

