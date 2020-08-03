#ifndef SERIAL_COMMANDS_H
#define SERIAL_COMMANDS_H

#include <stdint.h>

extern int32_t epoch_offset;

int toggle_led(int argc, char **argv);

int toggle_sensor(int argc, char **argv);

int time(int argc, char **argv);

int coap_get(int argc, char **argv);

int coap_post(int argc, char **argv);

#endif
