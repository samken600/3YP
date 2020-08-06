/*
 * Copyright (C) 2020 Samuel Kendall <sjk2g17@soton.ac.uk>
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

    /* find an temperature sensor device in the saul register */
    phydat_t res;
    dev = saul_reg_find_type(SAUL_SENSE_TEMP);
    if(dev == NULL) {
        puts("error: no temperature sensors found");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    /* read the sensor */
    int dim = saul_reg_read(dev, &res);
    if(dim <= 0) {
        puts("error: failed to read from device");
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    /* convert sensor reading to json format and set payload to this, return json to client */
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

    /* find an LED device in the saul register */
    dev = saul_reg_find_type(SAUL_ACT_SWITCH);
    if(dev == NULL) {
        puts("error: no led/switch found");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    /* ensure data buffer is zeroed */
    memset(&data, 0, sizeof(data));

    /* copy payload to local buffer. expect to be less than 16. might be better way to ensure this */
    char payload[16] = { 0 };
    memcpy(payload, (char*)pkt->payload, pkt->payload_len);

    /* parse payload and confirm returned a valid integer */
    /* anything negative or zero means off, anything positive means on */
    char *end;
    data.val[0] = (strtol(payload, &end, 10) > 0) ? 1 : 0;
    if(end==payload || *end != '\0' || errno == ERANGE) {
        puts("error: entered string is not number");
        return coap_reply_simple(pkt, COAP_CODE_UNSUPPORTED_CONTENT_FORMAT, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);

    }

    /* write value to LED*/
    dim = saul_reg_write(dev, &data);
    if(dim <= 0) {
        puts("error: failed to write to device");
        return coap_reply_simple(pkt, COAP_CODE_INTERNAL_SERVER_ERROR, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    /* return confirmation of change to client. no payload */

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

    /* get netif based on pid supplied in makefile. likely better way to do this */
    netif = gnrc_netif_get_by_pid(AT86RF2XX_PID);
    if(netif == NULL) {
        puts("error: netif not found on speicified PID");
        return coap_reply_simple(pkt, COAP_CODE_NOT_IMPLEMENTED, buf, len, 
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);
    }

    /* copy payload to local buffer. again, size only expects 16 */
    /* might be worth checking payload_len is less than this and error if not */
    char payload[16] = { 0 };
    memcpy(payload, (char*)pkt->payload, pkt->payload_len);

    /* parse incoming payload and ensure is a number */
    char *end;
    int16_t tx_power = strtol(payload, &end, 10);
    if(end==payload || *end != '\0' || errno == ERANGE) {
        puts("error: entered string is not number");
        return coap_reply_simple(pkt, COAP_CODE_UNSUPPORTED_CONTENT_FORMAT, buf, len,
                                 COAP_FORMAT_TEXT, (uint8_t*)rsp, p);

    }

    /* set txpower in transciever driver */
    netif->dev->driver->set(netif->dev,
                            NETOPT_TX_POWER,
                            &tx_power,
                            sizeof(tx_power));

    /* return confirmation of change */
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
