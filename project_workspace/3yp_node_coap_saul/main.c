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
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"

#define COAP_ADDR {0xfd, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}

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

int toggle_led(int argc, char **argv) {
    (void)argc;
    (void)argv;

    LED0_TOGGLE;
    return 0;
}

int toggle_sensor(int argc, char **argv) {
    (void)argc;
    (void)argv;

    SENSOR_POWER_TOGGLE;
    return 0;
}

int coap_send(int argc, char **argv) {
    (void)argc;
    (void)argv;

    const char *path = "temp";
    uint8_t buf[32];
    size_t len = sizeof(buf);
//    ipv6_addr_t result;
//    ipv6_addr_from_str(&result, "fe80::44bb:e7ff:fe54:68c1");
//    fd00:dead:beef::1
    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6, .addr={.ipv6=COAP_ADDR} };

    ssize_t res = nanocoap_get(&remote, path, buf, len);

    if(res < 0) {
        printf("error: %d", res);
        return res;
    }

    puts("success");
    //coap_pkt_t *pkt = (coap_pkt_t*)buf;

    printf("payload is: %s\n", buf);
    return res;
}

int coap_put(int argc, char **argv) {
    (void)argc;
    (void)argv;

    coap_pkt_t pkt;
    ssize_t res, len;
    uint8_t buf[32];
    //uint8_t *pktpos = buf;
    const char str[5] = "data";

    pkt.hdr = (coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_PUT, 1);
        
    coap_pkt_init(&pkt, buf, sizeof(buf), hdrlen);
    coap_opt_add_uri_path(&pkt, "temp");
    coap_opt_add_format(&pkt, COAP_FORMAT_TEXT);
    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    pkt.payload_len = strlen(str);
    memcpy(pkt.payload, str, pkt.payload_len);
    len += pkt.payload_len;

    /*coap_opt_put_uri_path(pktpos, 0, "/temp");
    coap_put_option_ct(pktpos, COAP_OPT_URI_PATH, COAP_FORMAT_TEXT);
    pkt.payload = str;
    pkt.payload_len = sizeof(str);*/

    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6, .addr={.ipv6=COAP_ADDR} };

    res = nanocoap_request(&pkt, NULL, &remote, sizeof(buf));
    if(res < 0) {
        printf("error %d", res);
        return res;
    }
    printf("success");
    return res;
}

static const shell_command_t shell_commands[] = {
    { "coap_put", "does a coap put", coap_put },
    { "coap_send", "sends coap get to laptop", coap_send },
    { "toggle_led", "toggles LED", toggle_led },
    { "toggle_sensor", "toggles sensor power", toggle_sensor },
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

