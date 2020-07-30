/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "xtimer.h"
#include "board.h"
#include "phydat.h"
#include "saul_reg.h"
#include "net/gnrc/netif.h"

#include "fmt.h"
#include "net/nanocoap.h"

static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}


static ssize_t _temp_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context) {
    (void) context;

    ssize_t p = 0;
    char rsp[64];
    unsigned code = COAP_CODE_EMPTY;
    saul_reg_t *dev;

    // get temp in celsius
    SENSOR_POWER_ON;
    xtimer_usleep(250000);

    phydat_t res;
    dev = saul_reg_find_type(SAUL_SENSE_TEMP);
    if(dev == NULL) {
        puts("error: no temperature sensors found");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    int dim = saul_reg_read(dev, &res);
    if(dim <= 0) {
        puts("error: failed to read from device");
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    SENSOR_POWER_OFF;
    
    p = phydat_to_json(&res, dim, rsp);
    code = COAP_CODE_CONTENT;
    
    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_JSON, (uint8_t*)rsp, p);
}

static ssize_t _led_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context) {
    (void)context;

    int dim;
    ssize_t p = 0;
    char rsp[16];
    unsigned code = COAP_CODE_EMPTY;
    saul_reg_t *dev;
    phydat_t data;

    dev = saul_reg_find_type(SAUL_ACT_SWITCH);
    if(dev == NULL) {
        puts("error: no led/switch found");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    memset(&data, 0, sizeof(data));
    char payload[16] = { 0 };
    memcpy(payload, (char*)pkt->payload, pkt->payload_len);

    char *end;
    data.val[0] = (strtol(payload, &end, 10) > 0) ? 1 : 0;
    if(end==payload || *end != '\0' || errno == ERANGE) {
        puts("error: entered string is not number");
        return coap_reply_simple(pkt, COAP_CODE_UNSUPPORTED_CONTENT_FORMAT, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);

    }
    dim = saul_reg_write(dev, &data);

    if(dim <= 0) {
        puts("error: failed to write to device");
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    code = COAP_CODE_CHANGED;

    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
}

static ssize_t _txpow_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context) {
    (void)context;

    ssize_t p = 0;
    char rsp[16];
    unsigned code = COAP_CODE_EMPTY;
    gnrc_netif_t *netif;
    phydat_t data;

    netif = gnrc_netif_get_by_pid(AT86RF2XX_PID);
    if(netif == NULL) {
        puts("error: netif not found on speicified PID");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    memset(&data, 0, sizeof(data));
    char payload[16] = { 0 };
    memcpy(payload, (char*)pkt->payload, pkt->payload_len);

    char *end;
    int16_t tx_power = strtol(payload, &end, 10);
    if(end==payload || *end != '\0' || errno == ERANGE) {
        puts("error: entered string is not number");
        return coap_reply_simple(pkt, COAP_CODE_UNSUPPORTED_CONTENT_FORMAT, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);

    }

    netif->dev->driver->set(netif->dev,
                            NETOPT_TX_POWER,
                            &tx_power,
                            sizeof(tx_power));

    code = COAP_CODE_CHANGED;

    return coap_reply_simple(pkt, code, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/led", COAP_POST | COAP_PUT, _led_handler, NULL },
    { "/riot/board", COAP_GET, _riot_board_handler, NULL },
    { "/temp", COAP_GET, _temp_handler, NULL },
    { "/txpower", COAP_PUT, _txpow_handler, NULL },
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
