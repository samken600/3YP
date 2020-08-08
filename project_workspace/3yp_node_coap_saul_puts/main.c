/*
 * Copyright (C) 2020 Samuel Kendall <sjk2g17@soton.ac.uk>
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
 * @brief       CoAP temperature server/client application
 *
 * @author      Samuel Kendall <sjk2g17@soton.ac.uk>
 * @}
 */

#include <stdio.h>

//#include "periph/pm.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"
#include "ztimer.h"
#include "shell.h"
#include "msg.h"

#include "serial_commands.h"
#include "util.h"

#define EPOCH_MSG 0
#define TEMP_MSG  1

#define DEFAULT_EPOCH_PERIOD 5 * SEC_PER_MIN * US_PER_SEC
#define DEFAULT_TEMP_PERIOD  2 * SEC_PER_MIN * US_PER_SEC

#define HANDLER_QUEUE_SIZE (8)
#define COAP_INBUF_SIZE    (256U)
#define MAIN_QUEUE_SIZE    (16)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static msg_t _handler_msg_queue[HANDLER_QUEUE_SIZE];

int32_t epoch_offset;
xtimer_t xt_get, xt_put;
uint32_t epoch_period, temp_period;

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

/* callback tells handler thread to get the epoch time */
void epoch_timer_cb(void *arg) {
    kernel_pid_t* pid = arg;

    msg_t msg;
    msg.content.value = EPOCH_MSG;
    
    int rsp = msg_try_send(&msg, *pid);

    xtimer_set(&xt_get, epoch_period);
}

/* callback tells handler thread to POST the temperature */
void temp_timer_cb(void *arg) {
    kernel_pid_t* pid = arg;

    msg_t msg;
    msg.content.value = TEMP_MSG;
    
    int rsp = msg_try_send(&msg, *pid);

    xtimer_set(&xt_put, temp_period);
}

/* thread handles CoAP client operations */
char handler_thread_stack[THREAD_STACKSIZE_MAIN];
void *handler_thread(void *arg) {
    (void)arg;

    /* use queue to prevent message collisions and lost messages */
    msg_init_queue(_handler_msg_queue, HANDLER_QUEUE_SIZE);
    
    msg_t msg;
    while(1) {
        msg_receive(&msg);
        //printf("value is %ld\n", msg.content.value);
        if(msg.content.value == EPOCH_MSG) update_epoch();
        else if(msg.content.value == TEMP_MSG) get_reading();
        else puts("Unknown message");
    }

    /* should never be reached */
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

    /* initialise default periods */
    epoch_period = DEFAULT_EPOCH_PERIOD;
    temp_period  = DEFAULT_TEMP_PERIOD;

    /* start handler thread */
    kernel_pid_t pid = thread_create(handler_thread_stack, sizeof(handler_thread_stack),
                                     THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                                     handler_thread, NULL, "handler_thread");

    /* ensure that epoch offset is set correctly on boot. twice to lower chance of error */
    update_epoch();
    xtimer_sleep(1);
    update_epoch();

    /* configure and start timers to determine when PUT and GET requests are made */
    xt_get.callback = epoch_timer_cb;
    xt_get.arg = &pid;
    xtimer_set(&xt_get, epoch_period);
    
    xt_put.callback = temp_timer_cb;
    xt_put.arg = &pid;
    xtimer_set(&xt_put, temp_period);

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

