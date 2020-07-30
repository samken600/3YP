#ifndef UTIL_H
#define UTIL_H

#include "fmt.h"
#include "net/nanocoap_sock.h"
#include "xtimer.h"

#define PHYSICAL_NETIF 6
#define COAP_ADDR {0xfd, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}

ssize_t get_hwaddr(uint8_t *hwaddr);

ssize_t coap_get_uri_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, ssize_t len);

ssize_t coap_post_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, const char *payload, ssize_t len, ssize_t payload_len);

#endif
