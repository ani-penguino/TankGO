#ifndef _JOYSTICK_H
#define _JOYSTICK_H

#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

struct joystick_admin {
    struct libusb_device_handle *joystick1;
    struct libusb_device_handle *joystick2;
    uint8_t joystick1_addr;
    uint8_t joystick2_addr;
};

struct joystick_packet {

    uint8_t c1;
    uint8_t c2;
    uint8_t c3;
    uint8_t h_dirc;
    uint8_t v_dirc;
    uint8_t xyab;
    uint8_t rl;
};

struct args_list {
    struct joystick_admin joysticks;
    char *buttons;
    int mode;
    int print;
};

extern struct joystick_admin open_controllers();

#endif

