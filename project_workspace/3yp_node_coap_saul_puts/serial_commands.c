#include "serial_commands.h"

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

int coap_get(int argc, char **argv) {
    (void)argc;
    (void)argv;

    return update_epoch();
}

int coap_post(int argc, char **argv) {
    (void)argc;
    (void)argv;

    uint8_t buf[256];
    char str[128];
    uint8_t ipv6[16] = COAP_ADDR;

    float temperature = 23.42;
    uint32_t time = epoch_offset + (xtimer_now_usec64()/1000000);
    uint8_t hwaddr[GNRC_NETIF_L2ADDR_MAXLEN];
    ssize_t addr_len = get_hwaddr(hwaddr);
    char addr_str[addr_len*3];

    ssize_t str_len = sprintf(str, "{\"hwaddr\": \"%s\", \"temp\": %.2f, \"time\": %ld}", gnrc_netif_addr_to_str(hwaddr, addr_len, addr_str), temperature, time);
//    printf("Payload is: %s, bytes: %d", str, str_len);

    ssize_t res = coap_post_via_proxy("coap://178.128.43.8/temp", ipv6, buf, str, sizeof(buf), str_len);

    if(res > 0) {
        printf("payload is: %s    bytes: %d\n", buf, res);
        puts("success");
    }
    else {
        printf("error: %d\n", res); 
    }
    return res;

}

