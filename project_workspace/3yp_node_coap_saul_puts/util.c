#include "util.h"
#include "phydat.h"
#include "saul_reg.h"
#include "fmt.h"
#include "net/nanocoap_sock.h"
#include "ztimer.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* create and initialise reading_list */
static reading_list_t reading_list = {.length = 0, .head = NULL};

/* get hwaddr of netif and save in hwaddr, return address length */
static ssize_t get_hwaddr(uint8_t *hwaddr);
/* get temperature via saul and store reading in res. returns pointer to result */
static phydat_t *get_temp(phydat_t *res);
/* sends get request to uri via proxy_addr. stores response in buf, which is of size len */
static ssize_t coap_get_uri_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, ssize_t len);
/* sends post request to uri via proxy_addr. stores response in buf, which is of size len. payload is of size payload_len */
static ssize_t coap_post_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, const char *payload, ssize_t len, ssize_t payload_len);
/* post temperature reading stored in node to server */
static ssize_t send_reading(reading_node_t *node);

static ssize_t get_hwaddr(uint8_t *hwaddr) {
    
    /* get netif based on pid */
    gnrc_netif_t *iface;
    iface = gnrc_netif_get_by_pid(PHYSICAL_NETIF);
    if(iface == NULL) {
        DEBUG("Is radio on pid %d?\n", PHYSICAL_NETIF);
        return 0;
    }

    /* extract hwaddr from netif */
    memcpy(hwaddr, iface->l2addr, iface->l2addr_len);
    char addr_str[(iface->l2addr_len)*3];
    DEBUG("HWAddr: %s\n", gnrc_netif_addr_to_str(hwaddr, iface->l2addr_len, addr_str));
    return iface->l2addr_len;
}    

static phydat_t *get_temp(phydat_t *res) {
    /* turn on sensor and wait for 1st reading */
    SENSOR_POWER_ON;
    xtimer_usleep(250000);

    /* find an temperature sensor device in the saul register */
    saul_reg_t *dev = saul_reg_find_type(SAUL_SENSE_TEMP);
    if(dev == NULL) {
        DEBUG("error: no temperature sensors found\n");
        return NULL;
    }

    /* read the sensor */
    int dim = saul_reg_read(dev, res);
    if(dim <= 0) {
        DEBUG("error: failed to read from device\n");
        return NULL;
    }

    SENSOR_POWER_OFF;
    return res;
}

static ssize_t coap_get_uri_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, ssize_t len) {
    coap_pkt_t pkt;
    ssize_t pkt_len, res;

    /* using supplied header as coap packet, build header */
    pkt.hdr=(coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);
    
    /* init packet body. add in proxy uri, and specify no payload */
    coap_pkt_init(&pkt, buf, len, hdrlen);
    coap_opt_add_proxy_uri(&pkt, uri);
    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    (void)pkt_len;

    /* configure local udp socket to send request from */
    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6 };
    memcpy(remote.addr.ipv6, proxy_addr, 16);

    /* send request then handle errors */
    res = nanocoap_request(&pkt, NULL, &remote, len);
    if(res < 0) {
        DEBUG("error: %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        /* if no error, set buf to return payload */
        if(pkt.payload_len) {
            memmove(buf, pkt.payload, pkt.payload_len);
            buf[pkt.payload_len] = '\0';
        }
        /* res is only positive if got CONTENT response, else is negative */
        if(res != 205) {
            res = -res;
        }
        else {
            res = pkt.payload_len;
            DEBUG("success\n");
        }
    }

    return res;
}

static ssize_t coap_post_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, const char *payload, ssize_t len, ssize_t payload_len) {

    coap_pkt_t pkt;
    ssize_t res, pkt_len;

    /* using supplied header as coap packet, build header */
    pkt.hdr = (coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_POST, 1);
        
    /* init packet body. add in proxy uri and specify format of payload*/
    coap_pkt_init(&pkt, buf, len, hdrlen);
    coap_opt_add_format(&pkt, COAP_FORMAT_JSON);
    coap_opt_add_proxy_uri(&pkt, uri);
    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    /* copy payload into packet */
    pkt.payload_len = payload_len;
    memcpy(pkt.payload, payload, pkt.payload_len);
    pkt_len += pkt.payload_len;

    /* configure local udp socket to send request from */
    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6 };
    memcpy(remote.addr.ipv6, proxy_addr, 16);

    /* send request then handle errors */
    res = nanocoap_request(&pkt, NULL, &remote, len);
    if(res < 0) {
        DEBUG("error %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        /* if no error, set buf to return payload */
        if(pkt.payload_len) {
            memmove(buf, pkt.payload, pkt.payload_len);
            buf[pkt.payload_len] = '\0';
        }
        /* res is only positive if got CREATED response, else is negative */
        if(res != 201) {
            res = -res;
        }
        else {
            res = pkt.payload_len;
        }
    }
    return res;
}

ssize_t update_epoch(void) {
    uint8_t buf[128];
    uint8_t ipv6[16] = COAP_ADDR;

    /* send get request to uri */
    ssize_t res = coap_get_uri_via_proxy("coap://178.128.43.8/time", ipv6, buf, sizeof(buf));

    /* check for errors in get request */    
    if(res > 0) {
        DEBUG("payload is: %s    bytes: %d\n", buf, res);
    }
    else {
        DEBUG("error: %d\n", res); 
        return res;
    }

    /* parse returned payload for time */
    uint8_t *end;
    uint32_t epoch = strtoul((char*)buf, (char**)&end, 10);
    if(end==buf || *end != '\0' || errno == ERANGE) {
        DEBUG("error: entered string is not number\n");
        return res;
    }

    /* set stored offset of clock, in seconds */
    epoch_offset = epoch - (ztimer_now(ZTIMER_MSEC) / MS_PER_SEC);
    DEBUG("epoch offset %ld\n", epoch_offset);
    return res;
}

static ssize_t send_reading(reading_node_t *node) {
    uint8_t buf[256];
    char str[128];
    uint8_t ipv6[16] = COAP_ADDR;

    /* get temperature from node and store in string */
    phydat_t temperature = node->temperature;
    char temp_str[8];
    uint8_t temp_str_len = fmt_s16_dfp(temp_str, temperature.val[0], (int)temperature.scale);
    temp_str[temp_str_len] = '\0';

    
    /* get time from node and get hwaddr */
    uint32_t time = node->time;
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    ssize_t addr_len = get_hwaddr(hwaddr);
    char addr_str[addr_len*3];

    /* produce formatted json string */
    ssize_t str_len = sprintf(str, "{\"hwaddr\": \"%s\", \"temp\": %s, \"time\": %ld}", gnrc_netif_addr_to_str(hwaddr, addr_len, addr_str), temp_str, time);
    DEBUG("Payload is: %s, bytes: %d\n", str, str_len);

    /* post json string to uri specified */
    ssize_t res = coap_post_via_proxy("coap://178.128.43.8/temp", ipv6, buf, str, sizeof(buf), str_len);

    /* check return code of post request and remove from queue if needed */
    if(res > 0) {
        DEBUG("payload is: %s    bytes: %d\n", buf, res);
        DEBUG("success\n");
        reading_remove(&reading_list, node);
    }
    else if(res == -406){
        DEBUG("packet was DUP, code: %d, assume already received\n", res); 
        reading_remove(&reading_list, node);
    }
    else {
        if(res == -400) {
            DEBUG("bad request, assume packet corruption so resend\n");
        }
        DEBUG("an error occured sending temp packet, keeping for retransmission...\n");
    }
    return res;
}

ssize_t get_reading(void) {
    phydat_t temperature;
    /* get temperature reading and check it is valid */
    if(get_temp(&temperature) == NULL || temperature.unit != UNIT_TEMP_C) return -1;

    /* generate timestamp immediately after taking reading */
    uint32_t time = epoch_offset + (ztimer_now(ZTIMER_MSEC) / MS_PER_SEC);

    /* add reading to the queue */
    DEBUG("Adding to queue\n");
    reading_add(&reading_list, temperature, time);
    reading_node_t *temp = reading_list.head;

    /* loop through first MAX_SENDS_PER_PERIOD nodes and try to send if they exist*/
    for(int i = 0; i<MAX_SENDS_PER_PERIOD; i++) {
        if(temp == NULL) break;
        send_reading(temp);
        DEBUG("Queue pos %d\n", i);
        temp = temp->next;
    }
    DEBUG("Length now %d\n", reading_list.length);

    return 0;    
}

void set_period(xtimer_t *xt, const uint32_t offset, uint32_t *period) {
    /* remove the timer, change ther period, then set it again */
    xtimer_remove(xt);
    *period = offset;

    xtimer_set(xt, *period);

    return;
}
