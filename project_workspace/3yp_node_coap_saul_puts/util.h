#ifndef UTIL_H
#define UTIL_H

#define PHYSICAL_NETIF 6
#define COAP_ADDR {0xfd, 0x00, 0xde, 0xad, 0xbe, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}

extern int32_t epoch_offset;

ssize_t get_hwaddr(uint8_t *hwaddr);

phydat_t *get_temp(phydat_t *res);

ssize_t coap_get_uri_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, ssize_t len);

ssize_t coap_post_via_proxy(const char *uri, const uint8_t *proxy_addr, uint8_t *buf, const char *payload, ssize_t len, ssize_t payload_len);

ssize_t update_epoch(void);

ssize_t send_temp(void);

void set_period(xtimer_t *xt, const uint32_t offset, uint32_t *period);

#endif
