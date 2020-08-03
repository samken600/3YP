#include "serial_commands.h"
#include "board.h"
#include "fmt.h"
#include "ztimer.h"
#include "util.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

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
    fmt_u32_dec(str, ztimer_now(ZTIMER_MSEC));
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

    return get_reading();
}

