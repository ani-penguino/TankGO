#ifndef _VGA_BALL_H
#define _VGA_BALL_H

#include <linux/ioctl.h>

typedef struct {
	unsigned char game_info_1,game_info_2,game_info_3, tank1loc_x,tank1loc_y, tank2loc_x,
                tank2loc_y,tank1_dirc,tank2_dirc,bullet1loc_x,bullet1loc_y,bullet2loc_x,
                bullet2loc_y,bullet1_dirc, bullet2_dirc, explo_loc_x,explo_loc_y;
} vga_ball_color_t;
  

typedef struct {
  vga_ball_color_t background;
} vga_ball_arg_t;

#define VGA_BALL_MAGIC 'q'

/* ioctls and their arguments */
#define VGA_BALL_WRITE_BACKGROUND _IOW(VGA_BALL_MAGIC, 1, vga_ball_arg_t *)
#define VGA_BALL_READ_BACKGROUND  _IOR(VGA_BALL_MAGIC, 2, vga_ball_arg_t *)

#endif
