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

//#include "periph/pm.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"
#include "shell.h"
#include "msg.h"

#include "serial_commands.h"
#include "util.h"

#define EPOCH_PERIOD 10 * SEC_PER_MIN * US_PER_SEC

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
int32_t epoch_offset;
xtimer_t xt;

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

void epoch_timer_cb(void *arg) {
    kernel_pid_t* pid = arg;

    msg_t msg;
    msg.content.value = 0;
    
    msg_try_send(&msg, *pid);

    xtimer_set(&xt, EPOCH_PERIOD);
}

char epoch_thread_stack[THREAD_STACKSIZE_MAIN];
void *epoch_thread(void *arg) {
    (void)arg;
    
    msg_t msg;
    while(1) {
        msg_receive(&msg);
        update_epoch();
    }

    return NULL;
}

static const shell_command_t shell_commands[] = {
    { "coap_post", "does a coap post", coap_post },
    { "coap_get", "sends coap get to laptop", coap_get },
    { "toggle_led", "toggles LED", toggle_led },
    { "toggle_sensor", "toggles sensor power", toggle_sensor },
    { "time", "displays time", time },
    { NULL, NULL, NULL }
};


int main(void)
{
    puts("RIOT nanocoap example application with shell and si70xx temperature sensor");

    /* sensor is on automatically */

    /* nanocoap_server uses gnrc sock which uses gnrc which needs a msg queue */
    msg_init_queue(_main_msg_queue, MAIN_QUEUE_SIZE);

    puts("Waiting for address autoconfiguration...");
    xtimer_sleep(3);

    kernel_pid_t pid = thread_create(epoch_thread_stack, sizeof(epoch_thread_stack),
                                     THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                                     epoch_thread, NULL, "epoch_thread");

    update_epoch();
    xt.callback = epoch_timer_cb;
    xt.arg = &pid;
    xtimer_set(&xt, EPOCH_PERIOD);

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

