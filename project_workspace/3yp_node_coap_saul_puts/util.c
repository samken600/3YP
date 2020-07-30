#include "util.h"

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

ssize_t coap_get_uri_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, ssize_t len) {
    coap_pkt_t pkt;
    ssize_t pkt_len, res;

    pkt.hdr=(coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_GET, 1);

    
    coap_pkt_init(&pkt, buf, len, hdrlen);
    coap_opt_add_proxy_uri(&pkt, uri);  //"coap://178.128.43.8/time");
    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_NONE);
    (void)pkt_len;

    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6 };
    memcpy(remote.addr.ipv6, proxy_addr, 16);

    res = nanocoap_request(&pkt, NULL, &remote, len);

    if(res < 0) {
        printf("error: %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if(pkt.payload_len) {
            memmove(buf, pkt.payload, pkt.payload_len);
            buf[pkt.payload_len] = '\0';
        }
        if(res != 205) {
            res = -res;
        }
        else {
            res = pkt.payload_len;
            puts("success\n");
        }
    }

    return res;
}

ssize_t coap_post_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, const char *payload, ssize_t len, ssize_t payload_len) {

    coap_pkt_t pkt;
    ssize_t res, pkt_len;

    pkt.hdr = (coap_hdr_t*)buf;
    ssize_t hdrlen = coap_build_hdr(pkt.hdr, COAP_TYPE_CON, NULL, 0, COAP_METHOD_POST, 1);
        
    coap_pkt_init(&pkt, buf, len, hdrlen);
    coap_opt_add_format(&pkt, COAP_FORMAT_JSON);
    coap_opt_add_proxy_uri(&pkt, uri);
    pkt_len = coap_opt_finish(&pkt, COAP_OPT_FINISH_PAYLOAD);

    pkt.payload_len = payload_len;
    memcpy(pkt.payload, payload, pkt.payload_len);
    pkt_len += pkt.payload_len;

    sock_udp_ep_t remote = { .port=COAP_PORT, .family=AF_INET6, .netif=6 };
    memcpy(remote.addr.ipv6, proxy_addr, 16);

    res = nanocoap_request(&pkt, NULL, &remote, len);
    if(res < 0) {
        printf("error %d\n", res);
        return res;
    }
    else {
        res = coap_get_code(&pkt);
        if(pkt.payload_len) {
            memmove(buf, pkt.payload, pkt.payload_len);
            buf[pkt.payload_len] = '\0';
        }
        if(res != 201) {
            res = -res;
        }
        else {
            res = pkt.payload_len;
        }
    }
    return res;
}

