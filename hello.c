#include <stdio.h>
#include "vga_ball.h"
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>
#include "joystick.h"
#include <pthread.h>

int vga_ball_fd;

vga_ball_color_t color;

int map = 0;

pthread_t pthread1, pthread2;

int tank1_hit = 0;
int tank2_hit = 0;
int bullet1_gone = 0;
int bullet2_gone = 0;

unsigned int tank1_score = 0;
unsigned int tank2_score = 0;

struct bullet_packet {
    unsigned char bulletloc_x;
    unsigned char bulletloc_y;
};

struct bullet_packet entry1;
struct bullet_packet entry2;



unsigned char game_info_1 = 0b00000000;
unsigned char game_info_2 = 0b00000000;
unsigned char game_info_3 = 0b00000000;
unsigned char tank1loc_x = 0b00010000;
unsigned char tank1loc_y = 0b01000100;
unsigned char tank2loc_x = 0b10001100;
unsigned char tank2loc_y = 0b01000100;
unsigned char tank1_dirc = 0b00000010;
unsigned char tank2_dirc = 0b00000110;
unsigned char bullet1loc_x = 0b00000000;
unsigned char bullet1loc_y = 0b00000000;
unsigned char bullet2loc_x = 0b00000000;
unsigned char bullet2loc_y = 0b00000000;
unsigned char bullet1_dirc = 0b00000000; // bullet enable
unsigned char bullet2_dirc = 0b00000000;
unsigned char explo_loc_x = 0b00000000;
unsigned char explo_loc_y = 0b00000000;

int bullet1_en = 0;
int bullet2_en = 0;

int map1[1200]=
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,1,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,1,
1,0,0,0,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

int map2[1200]=
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 
1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1, 
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0,1,
1,0,0,0,0,1,0,1,0,0,1,1,1,1,1,1,1,1,0,0,0,0,1,1,1,1,1,1,1,1,0,0,1,0,1,0,0,0,0,1,
1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,1,
1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,
1,0,0,0,0,1,0,0,0,0,1,1,1,1,0,0,1,1,0,0,0,0,1,1,0,0,1,1,1,1,0,0,0,0,1,0,0,0,0,1,
1,0,0,0,0,1,0,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,1,0,1,0,0,0,0,1,
1,0,0,0,0,1,1,1,0,0,1,0,0,0,0,0,1,1,1,0,0,1,1,1,0,0,0,0,0,1,0,0,1,1,1,0,0,0,0,1, 
1,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,0,1, 
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1, 
1,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,1, 
1,0,0,0,0,0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}; 

int map3[1200]=
{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,0,0,0,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,1,1,1,
1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,
1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,1,
1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,1,1,1,0,0,1,1,1,1,1,1,0,0,0,1,
1,0,0,0,0,0,1,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,1,0,0,1,1,1,1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,
1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,
1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,0,0,0,1,
1,1,1,1,0,0,1,0,0,1,0,0,1,0,0,1,0,0,1,1,1,1,0,0,1,0,0,1,0,0,1,1,1,1,1,1,1,1,1,1,
1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,
1,0,0,1,1,1,1,1,1,1,0,0,1,0,0,1,1,1,1,0,0,1,1,1,1,0,0,0,0,0,1,1,1,1,1,1,1,0,0,1,
1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};

/* Set the background color */
void set_background_color(const vga_ball_color_t *c){
    vga_ball_arg_t vla;
    vla.background = *c;
    if (ioctl(vga_ball_fd, VGA_BALL_WRITE_BACKGROUND, &vla)) {
        perror("ioctl(VGA_BALL_SET_BACKGROUND) failed");
        return;
    }
}

int collision_wall(char coordinate_x, char coordinate_y) {
    
    int x = (unsigned char)coordinate_x;
    int x1 = (unsigned char) coordinate_x + 1;
    int x2 = (unsigned char) coordinate_x + 2;
    int x3 = (unsigned char) coordinate_x + 3;
    int y = (unsigned char)coordinate_y;
    int y1 = (unsigned char) coordinate_y + 1;
    int y2 = (unsigned char) coordinate_y + 2;
    int y3 = (unsigned char) coordinate_y + 3;
    int block_x = x / 4;
    int block_y = y / 4;
    int block_x3 = x3 / 4;
    int block_y3 = y3 / 4;
    int map_index = block_x + block_y * 40;
    int map_index1 = block_x + block_y3 * 40;
    int map_index2 = block_x3 + block_y * 40;
    int map_index3 = block_x3 + block_y3 * 40;
    if (map == 0){
        if (map_index >= 0 && map_index < 1200 && (map1[map_index] == 1|| map1[map_index1] == 1 || map1[map_index2] == 1 || map1[map_index3] == 1)) {
            return 1;
        } else {
            return 0;
        }
    } else if (map == 1) {
        if (map_index >= 0 && map_index < 1200 && (map2[map_index] == 1|| map2[map_index1] == 1 || map2[map_index2] == 1 || map2[map_index3] == 1)) {
            return 1;
        } else {
            return 0;
        }
    } else if (map == 2) {
	if (map_index >= 0 && map_index < 1200 && (map3[map_index] == 1|| map3[map_index1] == 1 || map3[map_index2] == 1 || map3[map_index3] == 1)) {
            return 1;
        } else {
            return 0;
        }
    }

    
}

// coordinate 1 is bullet and coordinate 2 is tank.
int collision_tank(unsigned char coordinate_x_1, unsigned char coordinate_y_1, unsigned char coordinate_x_2, unsigned char coordinate_y_2) {
    int x1 = coordinate_x_1;
    int y1 = coordinate_y_1;
    int x2 = coordinate_x_2;
    int y2 = coordinate_y_2;

    int tank_right = x2 + 5;
    int tank_bottom = y2 + 5;

    if (x1 < tank_right && x1 > x2 && y1 < tank_bottom && y1 > y2) {
        return 1;
    }
    return 0;
}

int collision_bullettank(unsigned char coordinate_x_1, unsigned char coordinate_y_1, unsigned char coordinate_x_2, unsigned char coordinate_y_2) {
    
    unsigned char tank_x_max = coordinate_x_2 + 3;
    unsigned char tank_y_min = coordinate_y_2 + 3;

    if (coordinate_x_1 >= coordinate_x_2 && coordinate_x_1 <= tank_x_max &&
        coordinate_y_1 <= tank_y_min && coordinate_y_1 >= coordinate_y_2) {
        return 1;
    }

    return 0; 
}

int collision_bulletwall(unsigned char coordinate_x, unsigned char coordinate_y){
    int x = (unsigned char) coordinate_x;
    int y = (unsigned char) coordinate_y;
    int block_x = x / 4;
    int block_y = y / 4;
    int map_index = block_x + block_y * 40;
    if (map == 0){
        if (map_index >= 0 && map_index < 1200 && map1[map_index] == 1) {
            return 1;
        } else {
            return 0;
        }
    } else if (map == 1) {
        if (map_index >= 0 && map_index < 1200 && map2[map_index] == 1) {
            return 1;
        } else {
            return 0;
        }
    } else if (map == 2) {
	if (map_index >= 0 && map_index < 1200 && map3[map_index] == 1) {
            return 1;
        } else {
            return 0;
        }
    }
}

void *fire_bullet1(void *entry1){

    int bullet1_wallhit = 0; 

    struct bullet_packet *bullet_packet = (struct bullet_packet *) entry1;
    unsigned char bullet1loc_x = bullet_packet->bulletloc_x;
    unsigned char bullet1loc_y = bullet_packet->bulletloc_y;
    unsigned char bullet1_dirc = tank1_dirc;

    if (tank1_dirc == 0b00000000){
        bullet1loc_x = tank1loc_x + 2;
        bullet1loc_y = tank1loc_y - 1;
    } else if (tank1_dirc == 0b00000001){
        bullet1loc_x = tank1loc_x + 4;
        bullet1loc_y = tank1loc_y - 1;
    } else if (tank1_dirc == 0b00000010){
        bullet1loc_x = tank1loc_x + 4;
        bullet1loc_y = tank1loc_y + 2;
    } else if (tank1_dirc == 0b00000011){
        bullet1loc_x = tank1loc_x + 4;
        bullet1loc_y = tank1loc_y + 4;
    } else if (tank1_dirc == 0b00000100){
        bullet1loc_x = tank1loc_x + 2;
        bullet1loc_y = tank1loc_y + 4;
    } else if (tank1_dirc == 0b00000101){
        bullet1loc_x = tank1loc_x - 1;
        bullet1loc_y = tank1loc_y + 4;
    } else if (tank1_dirc == 0b00000110){
        bullet1loc_x = tank1loc_x - 1;
        bullet1loc_y = tank1loc_y + 2;
    } else if (tank1_dirc == 0b00000111){
        bullet1loc_x = tank1loc_x - 1;
        bullet1loc_y = tank1loc_y - 1;
    } 
    color.bullet1loc_x = bullet1loc_x;
    color.bullet1loc_y = bullet1loc_y;
    set_background_color(&color);

    for (;;){
        if (bullet1_wallhit > 15){
            bullet1_gone = 1;
            break;
        } else if (bullet1_dirc == 0b00000000){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y - 1) == 1){
                bullet1_dirc = 0b00000100;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_y -= 1;
            }
        } else if (bullet1_dirc == 0b00000001){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x+1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000111;
                bullet1_wallhit += 1;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y-1) == 1){
                bullet1_dirc = 0b00000011;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x += 1;
                bullet1loc_y -= 1;
            }
        } else if (bullet1_dirc == 0b00000010){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x + 1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000110;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x += 1;
            }
        } else if (bullet1_dirc == 0b00000011){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x+1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000101;
                bullet1_wallhit += 1;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y+1) == 1){
                bullet1_dirc = 0b00000001;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x += 1;
                bullet1loc_y += 1;
            }
        } else if (bullet1_dirc == 0b00000100){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y + 1) == 1){
                bullet1_dirc = 0b00000000;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_y += 1;
            }
        } else if (bullet1_dirc == 0b00000101){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x-1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000011;
                bullet1_wallhit += 1;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y+1) == 1){
                bullet1_dirc = 0b00000111;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x -= 1;
                bullet1loc_y += 1;
            }
        } else if (bullet1_dirc == 0b00000110){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x-1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000010;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x -= 1;
            }
        } else if (bullet1_dirc == 0b00000111){
            if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet1loc_x, bullet1loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet1loc_x-1, bullet1loc_y) == 1){
                bullet1_dirc = 0b00000001;
                bullet1_wallhit += 1;
            } else if (collision_bulletwall(bullet1loc_x, bullet1loc_y-1) == 1){
                bullet1_dirc = 0b00000101;
                bullet1_wallhit += 1;
            } else {
                bullet1loc_x -= 1;
                bullet1loc_y -= 1;
            }
        } 
        usleep(17000);
        color.bullet1loc_x = bullet1loc_x;
        color.bullet1loc_y = bullet1loc_y;
        set_background_color(&color);
    }
}

// if break without any hits, bullet disappear 
// if break with tank 1 hit or tank2 hit, then do whatever needed end thread or smt 
void *fire_bullet2(void *entry2){

    int bullet2_wallhit = 0; 

    struct bullet_packet *bullet_packet = (struct bullet_packet *) entry2;
    unsigned char bullet2loc_x = bullet_packet->bulletloc_x;
    unsigned char bullet2loc_y = bullet_packet->bulletloc_y;
    unsigned char bullet2_dirc = tank2_dirc;

    if (tank2_dirc == 0b00000000){
        bullet2loc_x = tank2loc_x + 2;
        bullet2loc_y = tank2loc_y - 1;
    } else if (tank2_dirc == 0b00000001){
        bullet2loc_x = tank2loc_x + 4;
        bullet2loc_y = tank2loc_y - 1;
    } else if (tank2_dirc == 0b00000010){
        bullet2loc_x = tank2loc_x + 4;
        bullet2loc_y = tank2loc_y + 2;
    } else if (tank2_dirc == 0b00000011){
        bullet2loc_x = tank2loc_x + 4;
        bullet2loc_y = tank2loc_y + 4;
    } else if (tank2_dirc == 0b00000100){
        bullet2loc_x = tank2loc_x + 2;
        bullet2loc_y = tank2loc_y + 4;
    } else if (tank2_dirc == 0b00000101){
        bullet2loc_x = tank2loc_x - 1;
        bullet2loc_y = tank2loc_y + 4;
    } else if (tank2_dirc == 0b00000110){
        bullet2loc_x = tank2loc_x - 1;
        bullet2loc_y = tank2loc_y + 2;
    } else if (tank2_dirc == 0b00000111){
        bullet2loc_x = tank2loc_x - 1;
        bullet2loc_y = tank2loc_y - 1;
    } 
    color.bullet2loc_x = bullet2loc_x;
    color.bullet2loc_y = bullet2loc_y;
    set_background_color(&color);

    for (;;){
        if (bullet2_wallhit > 15){
            bullet2_gone = 1;
            break;
        } else if (bullet2_dirc == 0b00000000){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y - 1) == 1){
                bullet2_dirc = 0b00000100;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_y -= 1;
            }tank2_dirc = 0b00000000;
                    color.tank2_dirc = 0b00000000;
                    set_background_color(&color);

        } else if (bullet2_dirc == 0b00000001){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x+1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000111;
                bullet2_wallhit += 1;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y-1) == 1){
                bullet2_dirc = 0b00000011;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x += 1;
                bullet2loc_y -= 1;
            }
        } else if (bullet2_dirc == 0b00000010){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x + 1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000110;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x += 1;
            }
        } else if (bullet2_dirc == 0b00000011){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x+1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000101;
                bullet2_wallhit += 1;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y+1) == 1){
                bullet2_dirc = 0b00000001;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x += 1;
                bullet2loc_y += 1;
            }
        } else if (bullet2_dirc == 0b00000100){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y + 1) == 1){
                bullet2_dirc = 0b00000000;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_y += 1;
            }
        } else if (bullet2_dirc == 0b00000101){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x-1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000011;
                bullet2_wallhit += 1;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y+1) == 1){
                bullet2_dirc = 0b00000111;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x -= 1;
                bullet2loc_y += 1;
            }
        } else if (bullet2_dirc == 0b00000110){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x-1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000010;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x -= 1;
            }
        } else if (bullet2_dirc == 0b00000111){
            if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank1loc_x, tank1loc_y) == 1){
                tank1_hit = 1;
                break;
            } else if (collision_bullettank(bullet2loc_x, bullet2loc_y, tank2loc_x, tank2loc_y) == 1){
                tank2_hit = 1;
                break;
            } else if (collision_bulletwall(bullet2loc_x-1, bullet2loc_y) == 1){
                bullet2_dirc = 0b00000001;
                bullet2_wallhit += 1;
            } else if (collision_bulletwall(bullet2loc_x, bullet2loc_y-1) == 1){
                bullet2_dirc = 0b00000101;
                bullet2_wallhit += 1;
            } else {
                bullet2loc_x -= 1;
                bullet2loc_y -= 1;
            }
        } 
        usleep(17000);
        color.bullet2loc_x = bullet2loc_x;
        color.bullet2loc_y = bullet2loc_y;
        set_background_color(&color);
    }
}

int main(){
    
    vga_ball_arg_t vla;
    static const char filename[] = "/dev/vga_ball";

    static vga_ball_color_t colors[] = {
        { 0x00, 0x00, 0x00, 0x9f, 0x00, 0x9f, 0x00, 0x01 }, /* Red */
        { 0x00, 0xff, 0x00, 0x9f, 0x00, 0x9f, 0x00, 0x01 }, /* Green */
        { 0x00, 0x00, 0xff, 0x9f, 0x00, 0x9f, 0x00, 0x01 }, /* Blue */
        { 0xff, 0xff, 0x00, 0x9f, 0x00, 0x9f, 0x00, 0x01 }, /* Yellow */
        { 0x00, 0xff, 0xff, 0x90, 0x00, 0x9f, 0x00, 0x01 }, /* Cyan */
        { 0xff, 0x00, 0xff, 0xA0, 0x00, 0x9f, 0x00, 0x01 }, /* Magenta */
        { 0x80, 0x80, 0x80, 0xB0, 0x00, 0x9f, 0x00, 0x01 }, /* Gray */
        { 0x00, 0x00, 0x00, 0xC0, 0x00, 0x9f, 0x00, 0x01 }, /* Black */
        { 0xff, 0xff, 0xff, 0xD0, 0x00, 0x9f, 0x00, 0x01 }  /* White */
    };

    # define COLORS 9

    printf("TANK GO! userspace program started\n");
    if ( (vga_ball_fd = open(filename, O_RDWR)) == -1) {
        fprintf(stderr, "could not open %s\n", filename);
        return -1;
    }

    struct joystick_admin joysticks = open_controllers();

    struct joystick_packet joystick1;
    struct joystick_packet joystick2;

    int length1 = sizeof(joystick1);
    int length2 = sizeof(joystick2);

    int array1;
    int array2;
    
    for (;;){

        map = 0;

        int map_up = 0;
        int map_down = 0;

        int tank1_rotleft = 0;
        int tank1_rotright = 0;
        int tank1_n = 0;
        int tank1_ne = 0;
        int tank1_e = 0;
        int tank1_se = 0;
        int tank1_s = 0;
        int tank1_sw = 0;
        int tank1_w = 0;
        int tank1_nw = 0;
        int tank1_fire = 0;
        int tank1_score = 0;

        int tank2_rotleft = 0;
        int tank2_rotright = 0;
        int tank2_n = 0;
        int tank2_ne = 0;
        int tank2_e = 0;
        int tank2_se = 0;
        int tank2_s = 0;
        int tank2_sw = 0;
        int tank2_w = 0;
        int tank2_nw = 0;
        int tank2_fire = 0;
        int tank2_score = 0;

	game_info_1 = 0b00000000;
	game_info_2 = 0b00000000;
	game_info_3 = 0b00000000;
	tank1loc_x = 0b00010000;
	tank1loc_y = 0b01000100;
	tank2loc_x = 0b10001100;
	tank2loc_y = 0b01000100;
	tank1_dirc = 0b00000010;
	tank2_dirc = 0b00000110;
	bullet1loc_x = 0b00000000;
	bullet1loc_y = 0b00000000;
	bullet2loc_x = 0b00000000;
	bullet2loc_y = 0b00000000;
	bullet1_dirc = 0b00000000; // bullet enable
	bullet2_dirc = 0b00000000;
	explo_loc_x = 0b00000000;
	explo_loc_y = 0b00000000;
        
        color.game_info_1 = game_info_1;
        color.game_info_2 = game_info_2;
        color.game_info_3 = game_info_3;
        color.tank1loc_x = tank1loc_x;
        color.tank1loc_y = tank1loc_y;
        color.tank2loc_x = tank2loc_x;
        color.tank2loc_y = tank2loc_y;
        color.tank1_dirc = tank1_dirc;
        color.tank2_dirc = tank2_dirc;
        color.bullet1loc_x = bullet1loc_x;
        color.bullet1loc_y = bullet1loc_y;
        color.bullet2loc_x = bullet2loc_x;
        color.bullet2loc_y = bullet2loc_y;
        color.bullet1_dirc = bullet1_dirc;
        color.bullet2_dirc = bullet2_dirc;
        color.explo_loc_x = explo_loc_x;
        color.explo_loc_y = explo_loc_y;

        set_background_color(&color);

        for (;;) {

            libusb_interrupt_transfer(joysticks.joystick1, joysticks.joystick1_addr, (unsigned char *) &joystick1, length1, &array1, 0);

            if (array1 == 7){

                uint8_t button1 = joystick1.xyab;

                if (joystick1.v_dirc == 0) { // up

                    map_up += 1;
                    if (map_up >= 600){
                        map_up = 0;
                    }

                    if (game_info_1 == 0b00000000){
                        map = 1;
                        game_info_1 = 0b00000001;
			color.game_info_1 = game_info_1;
			set_background_color(&color);
			usleep(150000);
                    } else if (game_info_1 == 0b00000001){
                        map = 2;
                        game_info_1 = 0b00000010;
			usleep(150000);
                    }
                    color.game_info_1 = game_info_1;
                    set_background_color(&color);

                } else if (joystick1.v_dirc == 255) { // down

                    map_down += 1;
                    if (map_down >= 600){
                        map_down = 0;
                    }

                    if (game_info_1 == 0b00000010){
                        map = 1;
                        game_info_1 = 0b00000001;
			color.game_info_1 = game_info_1;
                        set_background_color(&color);
			usleep(150000);
                    } else if (game_info_1 == 0b00000001){
                        map = 0;
                        game_info_1 = 0b00000000;
			color.game_info_1 = game_info_1;
                        set_background_color(&color);
			usleep(150000);
                    } 

                    color.game_info_1 = game_info_1;
                    set_background_color(&color);

                } else if (button1 == 31) {
                    
                    tank1_fire += 1;
                    if (tank1_fire >= 600) {
                        tank1_fire = 0;
                    }

                    printf("Selected Map %d \n", map + 1);
                    break;
                }
            }
        }

        for (;;){

	    if (bullet1_gone == 1){
		pthread_cancel(pthread1);
		bullet1loc_x = 0b00000000;
		bullet1loc_y = 0b00000000;
		bullet1_en = 0;
		bullet1_gone = 0;
		color.bullet1loc_x = bullet1loc_x;
		color.bullet1loc_y = bullet1loc_y;
		set_background_color(&color);
	    }
	    if (bullet2_gone == 1){
                pthread_cancel(pthread2);
                bullet2loc_x = 0b00000000;
                bullet2loc_y = 0b00000000;
		bullet2_en = 0;
		bullet2_gone = 0;
                color.bullet2loc_x = bullet2loc_x;
                color.bullet2loc_y = bullet2loc_y;
                set_background_color(&color);
            }
	    if (tank1_hit == 1 || tank2_hit == 1){
                if (tank1_hit == 1){
                    tank1_score += 2;
                    tank1_hit = 0;
		    pthread_cancel(pthread1);
                    pthread_cancel(pthread2);

		    color.tank1_dirc = 0b00000000;
		    set_background_color(&color);
		    usleep(10000);
		    color.tank1_dirc = 0b00000001;
		    set_background_color(&color);
	  	    usleep(10000);
		    color.tank1_dirc = 0b00000010;
		    set_background_color(&color);
		    usleep(10000);
		    color.tank1_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
		    color.tank1_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000111;
                    set_background_color(&color);
                    usleep(10000);
		    color.tank1_dirc = 0b00000000;
		    set_background_color(&color);
		    usleep(10000);
                    color.tank1_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000111;
                    set_background_color(&color);
                    usleep(10000);
		    color.tank1_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000110;
		    usleep(10000);
		    color.tank1_dirc = 0b00000111;
		    usleep(10000);
		    color.tank1_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank1_dirc = 0b00000110;
		    usleep(10000);
		    color.tank1_dirc = 0b00000111;
		    usleep(10000);
		    tank1_dirc = 0b00000000;
                    color.tank1_dirc = 0b00000000;
                    set_background_color(&color);

                }
                if(tank2_hit == 1){
                    tank2_score += 2;
                    tank2_hit = 0;
		    pthread_cancel(pthread1);
                    pthread_cancel(pthread2);
   
		    color.tank2_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000111;
                    set_background_color(&color);
		    usleep(10000);
		    color.tank1_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000111;
                    set_background_color(&color);
		    usleep(10000);
		    color.tank2_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000111;
                    set_background_color(&color);
		    usleep(10000);
		    color.tank2_dirc = 0b00000000;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000001;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000010;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000011;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000100;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000101;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000110;
                    set_background_color(&color);
                    usleep(10000);
                    color.tank2_dirc = 0b00000111;
                    set_background_color(&color);
		    usleep(10000);
		    tank2_dirc = 0b00000000;
		    color.tank2_dirc = 0b00000000;
		    set_background_color(&color);
                }
      
		bullet1_en = 0;
		bullet2_en = 0;
                bullet1loc_x = 0b00000000;
                bullet1loc_y = 0b00000000;
                color.bullet1loc_x = bullet1loc_x;
                color.bullet1loc_y = bullet1loc_y;
		bullet2loc_x = 0b00000000;
                bullet2loc_y = 0b00000000;
                color.bullet2loc_x = bullet2loc_x;
                color.bullet2loc_y = bullet2loc_y;

                set_background_color(&color);
  		
		if (tank1_score >= 15 || tank2_score >= 15){
                    game_info_3 = 0b00000001;
		    color.game_info_3 = 0b00000001;
		    set_background_color(&color);
		    usleep(4000000);
		    break;
                }


                tank1_score &= 0x0F;
                tank2_score &= 0x0F;

                game_info_2 = (tank1_score << 4) | tank2_score;

                color.game_info_2 = game_info_2;
                color.game_info_3 = game_info_3;
                color.explo_loc_x = explo_loc_x;
                color.explo_loc_y = explo_loc_y;
                set_background_color(&color);

                usleep(300000);
                explo_loc_x = 0b00000000;
                explo_loc_y = 0b00000000;
                color.explo_loc_x = explo_loc_x;
                color.explo_loc_y = explo_loc_y;
                set_background_color(&color);
	    }
            libusb_interrupt_transfer(joysticks.joystick1, joysticks.joystick1_addr, (unsigned char *) &joystick1, length1, &array1, 0);
            libusb_interrupt_transfer(joysticks.joystick2, joysticks.joystick2_addr, (unsigned char *) &joystick2, length2, &array2, 0);

            if (array1 == 7 && array2 == 7){
                uint8_t button1 = joystick1.xyab;
                if (joystick1.h_dirc == 255){
                    tank1_rotright += 1;
                    if (tank1_rotright >= 600){
                        tank1_rotright = 0;
                    }
                    if (tank1_dirc == 0b00000000){
                        tank1_dirc = 0b00000001;
                    } else if (tank1_dirc == 0b00000001){
                        tank1_dirc = 0b00000010;
                    } else if (tank1_dirc == 0b00000010){
                        tank1_dirc = 0b00000011;
                    } else if (tank1_dirc == 0b00000011){
                        tank1_dirc = 0b00000100;
                    } else if (tank1_dirc == 0b00000100){
                        tank1_dirc = 0b00000101;
                    } else if (tank1_dirc == 0b00000101){
                        tank1_dirc = 0b00000110;
                    } else if (tank1_dirc == 0b00000110){
                        tank1_dirc = 0b00000111;
                    } else if (tank1_dirc == 0b00000111){
                        tank1_dirc = 0b00000000;
                    }
                    color.tank1_dirc = tank1_dirc;
                    set_background_color(&color);
                    usleep(200000);
                }
                if (joystick1.h_dirc == 0){
                    tank1_rotleft += 1;
                    if (tank1_rotleft >= 600){
                        tank1_rotleft = 0;
                    }
                    if (tank1_dirc == 0b00000000){
                        tank1_dirc = 0b00000111;
                    } else if (tank1_dirc == 0b00000111){
                        tank1_dirc = 0b00000110;
                    } else if (tank1_dirc == 0b00000110){
                        tank1_dirc = 0b00000101;
                    } else if (tank1_dirc == 0b00000101){
                        tank1_dirc = 0b00000100;
                    } else if (tank1_dirc == 0b00000100){
                        tank1_dirc = 0b00000011;
                    } else if (tank1_dirc == 0b00000011){
                        tank1_dirc = 0b00000010;
                    } else if (tank1_dirc == 0b00000010){
                        tank1_dirc = 0b00000001;
                    } else if (tank1_dirc == 0b00000001){
                        tank1_dirc = 0b00000000;
                    }
                    color.tank1_dirc = tank1_dirc;
                    set_background_color(&color);
                    usleep(200000);
                }
                if (joystick1.v_dirc == 255){ // reversing
                    if (tank1_dirc == 0b00000000){ 
                        tank1_n += 1;
                        if (tank1_n >= 300){
                            tank1_n = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                    } else if (tank1_dirc == 0b00000001){ 
                        tank1_ne += 1;
                        if (tank1_ne >= 300){
                            tank1_ne = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000010){ 
                        tank1_e += 1;
                        if (tank1_e >= 300){
                            tank1_e = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y) == 0){
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000011){ 
                        tank1_se += 1;
                        if (tank1_se >= 300){
                            tank1_se = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000100){ 
                        tank1_s += 1;
                        if (tank1_s >= 300){
                            tank1_s = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                    } else if (tank1_dirc == 0b00000101){ 
                        tank1_sw += 1;
                        if (tank1_sw >= 300){
                            tank1_sw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                            tank1loc_x += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000110){ 
                        tank1_w += 1;
                        if (tank1_w >= 300){
                            tank1_w = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y) == 0){
                            tank1loc_x += 1;
                        }
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000111){ 
                        tank1_nw += 1;
                        if (tank1_nw >= 300){
                            tank1_nw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                            tank1loc_x += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    }
                }
                if (joystick1.v_dirc == 0){ // drive
                    if (tank1_dirc == 0b00000000){ 
                        tank1_n += 1;
                        if (tank1_n >= 300){
                            tank1_n = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                    } else if (tank1_dirc == 0b00000001){ 
                        tank1_ne += 1;
                        if (tank1_ne >= 300){
                            tank1_ne = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                            tank1loc_x += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000010){ 
                        tank1_e += 1;
                        if (tank1_e >= 300){
                            tank1_e = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y) == 0){
                            tank1loc_x += 1;
                        }
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000011){ 
                        tank1_se += 1;
                        if (tank1_se >= 300){
                            tank1_se = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x+1, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                            tank1loc_x += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000100){ 
                        tank1_s += 1;
                        if (tank1_s >= 300){
                            tank1_s = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                    } else if (tank1_dirc == 0b00000101){ 
                        tank1_sw += 1;
                        if (tank1_sw >= 300){
                            tank1_sw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y+1) == 0){
                            tank1loc_y += 1;
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000110){ 
                        tank1_w += 1;
                        if (tank1_w >= 300){
                            tank1_w = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y) == 0){
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_x = tank1loc_x;
                    } else if (tank1_dirc == 0b00000111){ 
                        tank1_nw += 1;
                        if (tank1_nw >= 300){
                            tank1_nw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank1loc_x-1, tank1loc_y-1) == 0){
                            tank1loc_y -= 1;
                            tank1loc_x -= 1;
                        }
                        color.tank1loc_y = tank1loc_y;
                        color.tank1loc_x = tank1loc_x;
                    } 
                    set_background_color(&color);
                } else if (button1 == 47 || button1 == 63 || button1 == 111 || button1 == 127 || button1 == 175 || button1 == 191 || button1 == 239 || button1 == 255) {
                    
                    tank1_fire += 1;
                    if (tank1_fire >= 600) {
                        tank1_fire = 0;
                    }

                    if (bullet1_en == 0){
                        bullet1_en = 1;
                        pthread_create(&pthread1, NULL, &fire_bullet1, (void *) &entry1);
                    }
                }
                set_background_color(&color);
                uint8_t button2 = joystick2.xyab;
                if (joystick2.h_dirc == 255){
                    tank2_rotright += 1;
                    if (tank2_rotright >= 600){
                        tank2_rotright = 0;
                    }
                    if (tank2_dirc == 0b00000000){
                        tank2_dirc = 0b00000001;
                    } else if (tank2_dirc == 0b00000001){
                        tank2_dirc = 0b00000010;
                    } else if (tank2_dirc == 0b00000010){
                        tank2_dirc = 0b00000011;
                    } else if (tank2_dirc == 0b00000011){
                        tank2_dirc = 0b00000100;
                    } else if (tank2_dirc == 0b00000100){
                        tank2_dirc = 0b00000101;
                    } else if (tank2_dirc == 0b00000101){
                        tank2_dirc = 0b00000110;
                    } else if (tank2_dirc == 0b00000110){
                        tank2_dirc = 0b00000111;
                    } else if (tank2_dirc == 0b00000111){
                        tank2_dirc = 0b00000000;
                    }
                    color.tank2_dirc = tank2_dirc;
                    set_background_color(&color);
                    usleep(200000);
                }
                if (joystick2.h_dirc == 0){
                    tank2_rotleft += 1;
                    if (tank2_rotleft >= 600){
                        tank2_rotleft = 0;
                    }
                    if (tank2_dirc == 0b00000000){
                        tank2_dirc = 0b00000111;
                    } else if (tank2_dirc == 0b00000111){
                        tank2_dirc = 0b00000110;
                    } else if (tank2_dirc == 0b00000110){
                        tank2_dirc = 0b00000101;
                    } else if (tank2_dirc == 0b00000101){
                        tank2_dirc = 0b00000100;
                    } else if (tank2_dirc == 0b00000100){
                        tank2_dirc = 0b00000011;
                    } else if (tank2_dirc == 0b00000011){
                        tank2_dirc = 0b00000010;
                    } else if (tank2_dirc == 0b00000010){
                        tank2_dirc = 0b00000001;
                    } else if (tank2_dirc == 0b00000001){
                        tank2_dirc = 0b00000000;
                    }
                    color.tank2_dirc = tank2_dirc;
                    set_background_color(&color);
                    usleep(200000);
                }
                if (joystick2.v_dirc == 255){ // reversing
                    if (tank2_dirc == 0b00000000){ 
                        tank2_n += 1;
                        if (tank2_n >= 300){
                            tank2_n = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                    } else if (tank2_dirc == 0b00000001){ 
                        tank2_ne += 1;
                        if (tank2_ne >= 300){
                            tank2_ne = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000010){ 
                        tank2_e += 1;
                        if (tank2_e >= 300){
                            tank2_e = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y) == 0){
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000011){ 
                        tank2_se += 1;
                        if (tank2_se >= 300){
                            tank2_se = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000100){ 
                        tank2_s += 1;
                        if (tank2_s >= 300){
                            tank2_s = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                    } else if (tank2_dirc == 0b00000101){ 
                        tank2_sw += 1;
                        if (tank2_sw >= 300){
                            tank2_sw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                            tank2loc_x += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000110){ 
                        tank2_w += 1;
                        if (tank2_w >= 300){
                            tank2_w = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y) == 0){
                            tank2loc_x += 1;
                        }
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000111){ 
                        tank2_nw += 1;
                        if (tank2_nw >= 300){
                            tank2_nw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                            tank2loc_x += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    }
                    set_background_color(&color);
                }
                if (joystick2.v_dirc == 0){ // drive
                    if (tank2_dirc == 0b00000000){ 
                        tank2_n += 1;
                        if (tank2_n >= 300){
                            tank2_n = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                    } else if (tank2_dirc == 0b00000001){ 
                        tank2_ne += 1;
                        if (tank2_ne >= 300){
                            tank2_ne = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                            tank2loc_x += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000010){ 
                        tank2_e += 1;
                        if (tank2_e >= 300){
                            tank2_e = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y) == 0){
                            tank2loc_x += 1;
                        }
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000011){ 
                        tank2_se += 1;
                        if (tank2_se >= 300){
                            tank2_se = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x+1, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                            tank2loc_x += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000100){ 
                        tank2_s += 1;
                        if (tank2_s >= 300){
                            tank2_s = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                    } else if (tank2_dirc == 0b00000101){ 
                        tank2_sw += 1;
                        if (tank2_sw >= 300){
                            tank2_sw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y+1) == 0){
                            tank2loc_y += 1;
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000110){ 
                        tank2_w += 1;
                        if (tank2_w >= 300){
                            tank2_w = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y) == 0){
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_x = tank2loc_x;
                    } else if (tank2_dirc == 0b00000111){ 
                        tank2_nw += 1;
                        if (tank2_nw >= 300){
                            tank2_nw = 0;
                        }
                        usleep(70000);
                        if (collision_wall(tank2loc_x-1, tank2loc_y-1) == 0){
                            tank2loc_y -= 1;
                            tank2loc_x -= 1;
                        }
                        color.tank2loc_y = tank2loc_y;
                        color.tank2loc_x = tank2loc_x;
                    }
                    set_background_color(&color);
                } else if (button2 == 47 || button2 == 63 || button2 == 111 || button2 == 127 || button2 == 175 || button2 == 191 || button2 == 239 || button2 == 255) {
                    
                    tank2_fire += 1;
                    if (tank2_fire >= 600) {
                        tank2_fire = 0;
                    }

                    if (bullet2_en == 0){
                        bullet2_en = 1;
                        pthread_create(&pthread2, NULL, &fire_bullet2, (void *) &entry2);
                    }
                }
            }
        }
    }
    return 0;
}
