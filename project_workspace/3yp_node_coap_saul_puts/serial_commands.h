#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <stdint.h>

extern int32_t epoch_offset;

/* toggles status of LED */
int toggle_led(int argc, char **argv);

/* toggles sensor power */
int toggle_sensor(int argc, char **argv);

/* prints current time according to ztimer in ms */
int time(int argc, char **argv);

/* gets epoch time from server and saves offset */
int coap_get(int argc, char **argv);

/* makes a temperature reading and posts to server */
int coap_post(int argc, char **argv);

#endif
