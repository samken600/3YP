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
#include "net/gnrc/netif.h"
#include "xtimer.h"
#include "board.h"
#include "periph/gpio.h"
#include "shell.h"
#include "fmt.h"

#define PHYSICAL_NETIF 6

#define COAP_ADDR {0xfd, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define COAP_ADDR2  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xb2, 0x80, 0x2b, 0x08}
//178.128.43.8/temp
// coap://178.128.43.8/temp

#define COAP_INBUF_SIZE (256U)

#define MAIN_QUEUE_SIZE     (8)
static msg_t _main_msg_queue[MAIN_QUEUE_SIZE];
static int32_t epoch_offset;

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

int time(int argc, char **argv) {
    (void)argc;
    (void)argv;

    char str[16];
    fmt_u64_dec(str, xtimer_now_usec64());
    printf("Time: %s \n", str);
    return 0;
}

// hwaddr is last 64 bits of ipv6 for netif, but with +2 on top byte
ssize_t get_hwaddr(uint8_t *hwaddr) {
//    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    gnrc_netif_t *iface;
    iface = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(iface == NULL) {
        printf("Is radio on pid %d?\n", PHYSICAL_NETIF);
        return 0;
    }
    memcpy(hwaddr, iface->l2addr, iface->l2addr_len);
//    char addr_str[(iface->l2addr_len)*3];
//    printf("HWAddr: %s\n", gnrc_netif_addr_to_str(hwaddr, iface->l2addr_len, addr_str));
    return iface->l2addr_len;
}    

int coap_get(int argc, char **argv) {
    (void)argc;
    (void)argv;

    coap_pkt_t pkt;
    uint8_t buf[64];
    ssize_t len, res;

    pkt.hdr=(coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);

    coap_pkt_init(&pkt, buf, sizeof(buf), hdrlen);
    coap_opt_add_proxy_uri(&pkt, "coap://178.128.43.8/time");
    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    len += 0;

    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6, .addr={.ipv6=COAP_ADDR} };

    //res = nanocoap_get(&remote, path, buf, len);
    res = nanocoap_request(&pkt, NULL, &remote, sizeof(buf));

    if(res < 0) {
        printf("error: %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if(res != 205) {
            res = -res;
            printf("failed, error: %d\n", res);
            return res;
        }
        else {
            if(pkt.payload_len) {
                memmove(buf, pkt.payload, pkt.payload_len);
            }
            res = pkt.payload_len;
            puts("success\n");
        }
    }

    if(res > 0) {
        buf[res] = '\0';
        printf("payload is: %s    bytes: %d\n", buf, res);
    }

    uint8_t *end;
    uint32_t epoch = strtoul((char*)buf, (char**)&end, 10);
    if(end==buf || *end != '\0' || errno == ERANGE) {
        puts("error: entered string is not number");
        return res;
    }

    epoch_offset = epoch - (xtimer_now_usec64()/1000000);
    printf("epoch offset %ld\n", epoch_offset);

    return res;
}

int coap_post(int argc, char **argv) {
    (void)argc;
    (void)argv;

    coap_pkt_t pkt;
    ssize_t res, len;
    uint8_t buf[256];
    //uint8_t *pktpos = buf;
    char str[128];

    float temperature = 23.42;
    uint32_t time = epoch_offset + (xtimer_now_usec64()/1000000);
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    ssize_t addr_len = get_hwaddr(hwaddr);
    char addr_str[addr_len*3];

    ssize_t str_len = sprintf(str, "{\"hwaddr\": \"%s\", \"temp\": %.2f, \"time\": %ld}", gnrc_netif_addr_to_str(hwaddr, addr_len, addr_str), temperature, time);
//    printf("Payload is: %s, bytes: %d", str, str_len);

    pkt.hdr = (coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_POST, 1);
        
    coap_pkt_init(&pkt, buf, sizeof(buf), hdrlen);
    //coap_opt_add_uri_path(&pkt, "temp");
    coap_opt_add_format(&pkt, COAP_FORMAT_JSON);
    coap_opt_add_proxy_uri(&pkt, "coap://178.128.43.8/temp");
    len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    pkt.payload_len = str_len;
    memcpy(pkt.payload, str, pkt.payload_len);
    len += pkt.payload_len;

    /*coap_opt_put_uri_path(pktpos, 0, "/temp");
    coap_put_option_ct(pktpos, COAP_OPT_URI_PATH, COAP_FORMAT_TEXT);
    pkt.payload = str;
    pkt.payload_len = sizeof(str);*/

    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6, .addr={.ipv6=COAP_ADDR} };

    res = nanocoap_request(&pkt, NULL, &remote, sizeof(buf));
    if(res < 0) {
        printf("error %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if(res != 201) {
            res = -res;
        }
        else {
            if(pkt.payload_len) {
                memmove(buf, pkt.payload, pkt.payload_len);
            }
            res = pkt.payload_len;
            buf[res] = '\0';

            printf("payload is: %s    bytes: %d\n", buf, res);
            printf("success\n");
        }
    }
    return res;
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

